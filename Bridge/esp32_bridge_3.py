import serial
import keyboard
import time

# CRUCIAL: Set this to the NATIVE USB / OTG port number of your S3
COM_PORT = 'COM13'  
BAUD_RATE = 115200

print(f"Connecting Python to ESP32-S3 NATIVE OTG Port ({COM_PORT})...")
try:
    esp32 = serial.Serial(COM_PORT, BAUD_RATE, timeout=1)
    time.sleep(2)
    print("Connected successfully!")
except Exception as e:
    print(f"Error: {e}")
    exit()

print("\n--- Live Keystroke Streamer Active ---")
print("Streaming typing data to the S3 OTG line...")

def on_key_event(event):
    if event.event_type == keyboard.KEY_DOWN:
        if len(event.name) == 1:
            esp32.write(event.name.encode('utf-8'))
        elif event.name == 'space':
            esp32.write(b' ')

keyboard.hook(on_key_event)
keyboard.wait()
