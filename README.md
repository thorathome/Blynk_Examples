SONOFF_OTA Sketch README

The Arduino/Blynk sketch SONOFF_OTA_601_20210505.ino is a **fully-developed OTA-enabled, multi-device SONOFF control system** using the powerful BlynkSimpleESP8266_SSL_WM WiFiManager (WM) libraries for ESP8266. 

[SONOFF CONTROL directory contains the Arduino Blynk OTA code. Helper Files directory has required #include files.]

**If you are new to Arduino, Blynk, or SONOFFs, this probably is not the first Arduino sketch you should try.** It uses some advanced features and requires flashing a SONOFF. 

This sketch also demonstrates several Blynk, Blynk_WiFiManager, and Arduino IDE features.
* Basic Blynk_WiFiManager Config Portal configuration and use
* Wonderful OTA code from Andreas Spiess, The Guy With the Swiss Accent, from his YouTube video #332  _(I modified his code a bit to assume WiFi connection is already in place - set up by Blynk_WiFiManager. Modified code available in this repository. Thank you, Andreas Spiess.)_
* Use of multiple SONOFF devices on one Blynk Authcode
* Use of Blynk's ```BLYNK_WRITE_DEFAULT()``` flexible capability
* Use of compiler constants for compile-time device selection
* Use of EEPROM to remember the device labels in flash memory. _(LittleFS was too large for SONOFF's relatively small memory.)_

For each SONOFF device, the Blynk App wants to see...
* Blynk Button in SWITCH mode (ON/OFF)
* Blynk LED (Heartbeat)
* Blynk Value Display Widget (for the Switch Label)
* Blynk Text Input Widget (to set and display the Switch label... AND to force a Reboot)
* (Virtual Pins are hardcoded into the sketch, device by device, using #compiler directives.)
* **THEREFORE, for 6 SONOFF devices, you'll want 6 of each Widget.** _(I only have 1 Text Input Widget for all 6 SONOFFs, and I change the Vpin from the Blynk App when labelling or Rebooting the Switches.)_

Arduino IDE requires 
* Arduino ESP core installed (for the ESP8266-based SONOFFs)
* **Blynk master library 0.6.x (not the 1.0.0-beta version) installed**
* Blynk_WiFiManager library installed
* Several #include files placed in a folder in your library directory, downloaded for edit and use from this repository
* * MY_WIFI_CREDENTIALS.h - edit in your WiFi SSIDs and Passwords
* * MY_BLYNK_CREDENTIALS.h - edit in your Blynk Authcodes 
* * MY_BLYNK_WIFIMANAGER_SETUP_SSL.h - edit in you own preferences for standard Blynk_WiFiManager setup
* * MY_ESP_LED_BUILTINS.h - helpers for inverted ESP8266 LED logic
* * MY_BLYNK_COLORS.h - bunch of Blynk colors in hex
* * MY_SERIAL_PRINTS.h - helpers for Serial.print
* * OTABlynkTelNet.h or OTABlynk.h - Spiess' (slightly modified) OTA and OTA/TelNetStream code (See below for more info)

You will also need, of course
* One or more SONOFF Basic (ESP8266) devices and the ability to flash the SONOFFs. (See elsewhere for How Tos.)

As in all Blynk sketches using WiFi, you will need
* Your WiFi SSID or SSIDs and passwords
* Your Blynk authcode or authcodes
* Your Blynk server URL (I use blynk-cloud.com.)

The Blynk_WiFiManager has a Config Portal for entering WiFi and Blynk Server info at runtime. However, I hardcode this into the sketch, and can change it using the Config Portal if I ever need to. 

The Config Portal will appear as SSID MyConfigPortal, with WiFi password of 12345678
and IP address of 192.168.220.1  
These values (Config Portal SSID, password, IP address) are hardcoded into the sketch
using #include files. 

_For SONOFF development, I recommend working initially with a good standard ESP8266 to get your sketch logic, UI, and OTA right. Then learn to flash the SONOFF with your working sketch. Here's a link to a helpful tutorial on SONOFF flashing. There are others, too._
https://medium.com/@jeffreyroshan/flashing-a-custom-firmware-to-sonoff-wifi-switch-with-arduino-ide-402e5a2f77b _Once you have a working SONOFF with OTA, updating the sketch is much easier._
   
This repository https://github.com/thorathome/Blynk_Examples 
   
See also https://github.com/khoih-prog/Blynk_WM for the Blynk WiFi Manager library. (You can download it directly from the IDE.)

See Andreas Spiess on YouTube for his OTA episodes. This one refers to ESP32, but is also applicable to ESP8266/SONOFF.
https://youtu.be/1pwqS_NUG7Q

