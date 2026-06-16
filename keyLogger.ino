#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h> 
#include <WiFi.h>
#include <ESPAsyncWebServer.h> 
#include <AsyncTCP.h>          

// IdeaSpark 1.14" TFT Internal SPI Hardware Pins
#define TFT_CS         15
#define TFT_DC          2
#define TFT_RST         4
#define TFT_MOSI       23
#define TFT_SCLK       18
#define TFT_BLK        32 // Backlight Control Pin

// Define screen dimensions for the 1.14" ST7789 panel
#define SCREEN_WIDTH  135
#define SCREEN_HEIGHT 240

// Instantiate custom SPI bus to force the ESP32 to route to internal board traces
SPIClass tftSPI(HSPI);

// Pass our custom SPI bus to the display constructor
Adafruit_ST7789 display = Adafruit_ST7789(&tftSPI, TFT_CS, TFT_DC, TFT_RST);

// Private diagnostics network credentials (Open Network configuration)
const char* ssid = "HG_KeyLogger";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); 

// Reassigned DEVICE_PIN to the Backlight control line (GPIO 32)
const int DEVICE_PIN = 32; 
String inputBuffer = "";  
const int MAX_BUFFER_SIZE = 18; 

// Updated CSS styling using pre-wrap to retain spaces sent over the socket
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>Live Diagnostics Console</title>
    <style>
        body { font-family: monospace; background: #121212; color: #00ff00; padding: 20px; }
        h2 { color: #ffffff; border-bottom: 2px solid #333; padding-bottom: 10px; }
        #console { background: #000000; border: 1px solid #333; height: 300px; padding: 15px; overflow-y: scroll; font-size: 16px; border-radius: 5px; white-space: pre-wrap; }
        .trigger { color: #ff9900; font-weight: bold; }
    </style>
</head>
<body>
    <h2>ESP32 Live Diagnostics Panel</h2>
    <p>Status: <span id="status" style="color:red;">Disconnected</span></p>
    <div id="console"></div>

    <script>
        var gateway = `ws://${window.location.hostname}/ws`;
        var websocket;
        
        function initWebSocket() {
            websocket = new WebSocket(gateway);
            websocket.onopen = function() { document.getElementById('status').innerHTML = 'Connected Live'; document.getElementById('status').style.color = '#00ff00'; };
            websocket.onclose = function() { document.getElementById('status').innerHTML = 'Disconnected'; document.getElementById('status').style.color = 'red'; setTimeout(initWebSocket, 2000); };
            websocket.onmessage = function(event) {
                var consoleDiv = document.getElementById('console');
                consoleDiv.innerHTML += event.data;
                consoleDiv.scrollTop = consoleDiv.scrollHeight; 
            };
        }
        window.onload = initWebSocket;
    </script>
</body>
</html>
)rawliteral";

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {}

// updateDisplay draws a unified screen layout containing all info at once
void updateDisplay() {
  display.fillScreen(ST77XX_BLACK); 
  
  // ==========================================
  // 1. Main Title Header (Text Size 2)
  // Character height: 16 pixels. Position: Y = 0
  // ==========================================
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.setTextColor(ST77XX_GREEN);
  display.print("System Active"); // Changed to print() to control spacing
  
  // ==========================================
  // 2. Network Properties - AP Name (Text Size 2)
  // Character height: 16 pixels. 
  // Math: Line 1 (0) + Height (16) + 8px Gap = 24
  // ==========================================
  display.setTextColor(ST77XX_WHITE);
  display.setCursor(0, 24);
  display.print("AP: HG_KeyLogger");
  
  // ==========================================
  // 2. Network Properties - IP Address (Text Size 2)
  // Character height: 16 pixels.
  // Math: Line 2 (24) + Height (16) + 8px Gap = 48
  // ==========================================
  display.setCursor(0, 48);
  display.print("IP: 192.168.4.1");
  
  // ==========================================
  // 3. Clean line separator (Orange)
  // Math: Line 3 (48) + Height (16) + 8px Gap = 72
  // ==========================================
  display.drawFastHLine(0, 72, 240, ST77XX_ORANGE);
  
  // ==========================================
  // 4. User Input Console Area (Text Size 2)
  // Math: Divider (72) + 12px Gap for extra prominence = 84
  // ==========================================
  display.setCursor(0, 84); 
  display.print("> ");
  display.print(inputBuffer);
}

void setup() {
  Serial.begin(115200);   // Native USB Port (Python App Link)
  Serial0.begin(115200);  // Hardware UART Port (Arduino Debug Terminal)

  pinMode(DEVICE_PIN, OUTPUT);
  digitalWrite(DEVICE_PIN, HIGH);

  tftSPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS); 

  display.init(SCREEN_WIDTH, SCREEN_HEIGHT); 
  display.setRotation(1); // Landscape Mode
  
  Serial0.println("\n==============================================");
  Serial0.println("DEBUG CONSOLE: ESP32 TFT-Display System Ready");
  Serial0.println("==============================================");
  
  // Initialize Hotspot Network
  WiFi.softAP(ssid); 
  IPAddress IP = WiFi.softAPIP();
  
  ws.onEvent(onEvent);
  server.addHandler(&ws);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html);
  });
  server.begin();
  
  Serial0.print("Diagnostics Local IP Address: ");
  Serial0.println(IP.toString()); 

  // Direct layout initialization
  updateDisplay();
}

void loop() {
  ws.cleanupClients();

  while (Serial.available() > 0) {
    char incomingChar = Serial.read();
    
    if (isprint(incomingChar) || incomingChar == ' ') {
      inputBuffer += incomingChar;
      
      Serial0.print("Debug - Key Received: '");
      Serial0.print(incomingChar);
      Serial0.println("'");

      ws.textAll(String(incomingChar));
    }

    if (inputBuffer.length() > MAX_BUFFER_SIZE) {
      inputBuffer = inputBuffer.substring(inputBuffer.length() - MAX_BUFFER_SIZE);
    }

    inputBuffer.toUpperCase();
    updateDisplay(); 

    if (inputBuffer.endsWith("BLINK")) {
      Serial0.println("--> MATCH FOUND: Executing Backlight BLINK sequence");
      ws.textAll("\n<span class='trigger'>[ACTION: BLINKING BACKLIGHT]</span>\n");
      
      for(int i = 0; i < 6; i++) {
        digitalWrite(DEVICE_PIN, !digitalRead(DEVICE_PIN));
        delay(200);
      }
      digitalWrite(DEVICE_PIN, HIGH); 
      inputBuffer = "";
      updateDisplay();
    }
  }
}