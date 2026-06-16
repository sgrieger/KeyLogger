import serial
import keyboard
import time

# CHANGE THIS to match your ESP32-S3 port from the Arduino IDE
COM_PORT = 'COM8' 
BAUD_RATE = 115200

print(f"Connecting to ESP32-S3 on {COM_PORT}...")
try:
    esp32 = serial.Serial(COM_PORT, BAUD_RATE, timeout=1)
    time.sleep(2) 
    print("Connected successfully!")
except Exception as e:
    print(f"Error: {e}. Close Arduino Serial Monitor first.")
    exit()

print("\n--- Live Keystroke Streamer Active ---")
print("Every key you type anywhere on Windows is now being sent to the ESP32-S3!")
print("Type 'ON', 'OFF', or 'BLINK' naturally in any app (like Notepad).")
print("Press 'Ctrl + C' in this terminal to stop.")

def on_key_event(event):
    # Only capture when a key is pressed down (ignore key release)
    if event.event_type == keyboard.KEY_DOWN:
        # Ignore modifier keys like Shift, Ctrl, Alt to prevent garbage characters
        if len(event.name) == 1: 
            # Send the exact character directly down the USB wire
            esp32.write(event.name.encode('utf-8'))

# Hook into ALL Windows keyboard inputs globally
keyboard.hook(on_key_event)

# Keep the script running
keyboard.wait()
