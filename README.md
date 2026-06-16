# Simulated Key Logger for Cyber Security Students

## Bridge file
The file Bridge.exe needs to be running on the target computer. This file is actually what traps all the 
key presses and send them to the ESP32 microcontroller.

It is a python script compiled to an executable so it does not need Python installed.

The python script it is built on is in the Bridge folder if you need to make changes.

Once it is running, it will poll the com ports until it finds the microcontroller. At this point it will begin sending it keystrokes

## ESP32 
The microcontroller needs to be running the code that is in the keylogger.ino file.

Once the code is flashed to the ESP32, plug the microcontroller into the target computer and Bridge.exe will find it and begin sending it keystrokes.

Each keystroke will appear on the ESP32's screen as you type it in real time.

## Special command
Typing in the word "blink", case insensitive, it will flash the micro controller screen three times.

## WiFi AP
You can connect your phone to the WiFi access point that the microcontroller exposes. It's called "HG_KeyLogger" and there is no password.
Once connected, your phone will not have internet but you can browse to 192.168.4.1 to see the web page it renders.
All keys pressed on the target computer will now also appear on the webpage.

## Teaching 
1. You can go to a website that allows a Username and Password and demonstrate how even on a password field that shows *** as the password, that the key logger is able to see the password as you type it in in clear text.
2. You can explain that on a device like this, the bad actor can simply walk past the office, log into the device AP, download the data and keep walking. There is no more need to sit at that station ever again.


Again, this is simulated for deomstration purposes olny and as a teaching aid.
