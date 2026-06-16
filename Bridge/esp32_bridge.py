import serial
import keyboard
import time

# 1. CHANGE THIS to match your ESP32-S3 port from the Arduino IDE
COM_PORT = 'COM8' 
BAUD_RATE = 115200

print(f"Connecting to ESP32-S3 on {COM_PORT}...")
try:
    # Open the serial hardware pipe to the ESP32
    esp32 = serial.Serial(COM_PORT, BAUD_RATE, timeout=1)
    time.sleep(2) # Give the S3 native USB a moment to initialize
    print("Connected successfully!")
except Exception as e:
    print(f"Error connecting: {e}")
    print("Make sure your Arduino Serial Monitor is CLOSED, or the script can't connect.")
    exit()

print("\n--- Global Hotkey Listener Active ---")
print("Press 'Ctrl + Alt + 1' to turn ON")
print("Press 'Ctrl + Alt + 2' to turn OFF")
print("Press 'Ctrl + Alt + 3' to BLINK")
print("Press 'Ctrl + C' in this terminal window to exit.")

# 2. DEFINE YOUR ACTIONS
# When you press the hotkey, it writes the word followed by '\n' (Enter) over the USB wire
def send_on():
    print("Hotkey detected: Sending ON")
    esp32.write(b"ON\n")

def send_off():
    print("Hotkey detected: Sending OFF")
    esp32.write(b"OFF\n")

def send_blink():
    print("Hotkey detected: Sending BLINK")
    esp32.write(b"BLINK\n")

# 3. ASSIGN THE GLOBAL KEY COMBOS
# These work system-wide, even if you are playing a full-screen game or typing in Notepad
keyboard.add_hotkey('ctrl+alt+1', send_on)
keyboard.add_hotkey('ctrl+alt+2', send_off)
keyboard.add_hotkey('ctrl+alt+3', send_blink)

# Keep the background script running forever
keyboard.wait()
