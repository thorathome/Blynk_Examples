The Arduino/Blynk sketch SONOFF_OTA_601_20210505.ino.ino is a **fully-developed OTA-enabled, SONOFF control system** using the powerful BlynkSimpleESP8266_SSL_WM WiFiManager (WM) libraries for ESP8266. 

**If you are new to Arduino, Blynk, or SONOFFs, this probably is not the first Arduino sketch you should try. **It uses some advanced features and requires flashing a SONOFF. 

This sketch also demonstrates several Blynk, Blynk_WiFiManager, and Arduino features.
* Basic Blynk_WiFiManager Config Portal configuration and use
* Wonderful OTA code from Andreas Spiess, The Guy With the Swiss Accent, from his YouTube video #332  (I modified his code a bit to assume WiFi connection is already in place - done my Blynk_WiFiManager.)
* Use of EEPROM to remember the device labels in flash memory. (LittleFS was too large for SONOFF's relatively small memory.)
* Use of compiler constants for compile-time device selection
* Use of Blynk's BLYNK_WRITE_DEFAULT() flexible capability
* Use of multiple devices on one Blynk Authcode

For each SONOFF device, the Blynk App wants to see...
* Blynk Button in SWITCH mode (ON/OFF)
* Blynk LED (Heartbeat)
* Blynk Text Input Widget Display (to set and edisplay the Switch label... AND to force a Reboot)

Arduino IDE requires 
* Arduino ESPcore installed (for the ESP8266-based SONOFFs)
* Blynk master library 0.6.x (not the 1.0.0-beta version) installed
* Blynk_WiFiManager library installed

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
   
See https://github.com/thorathome/Blynk_Examples
   
See also https://github.com/khoih-prog/Blynk_WM for the Blynk WiFi Manager library

See Andreas Spiess on YouTube for his OTA episodes. This one refers to ESP32, but is also applicable to ESP8266/SONOFF.
https://youtu.be/1pwqS_NUG7Q

