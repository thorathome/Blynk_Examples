The Arduino/Blynk sketch SONOFF_OTA_601_20210505.ino.ino is a fully-developed OTA-enabled, SONOFF control system using the powerful BlynkSimpleESP8266_SSL_WM WiFiManager (WM) libraries for ESP8266. 

If you are new to Arduino, Blynk, or SONOFFs, this probably is not the first Arduino sketch you should try. It uses some advanced features and requires flashing a SONOFF. 

_For SONOFF development, I recommend working initially with a good standard ESP8266 to get your sketch logic, UI, and OTA right. Then learn to flash the SONOFF with working code. Here's a link to a helpful tutorial on SONOFF flashing. There are others, too._
https://medium.com/@jeffreyroshan/flashing-a-custom-firmware-to-sonoff-wifi-switch-with-arduino-ide-402e5a2f77b 
   
This sketch demonstrates some advanced Blynk and Arduino features.
* Basic WiFiManager Config Portal configuration and use
* Wonderful OTA code from Andreas Spiess, The Guy With the Swiss Accent, from his YouTube video #332  (I modified his code a bit to assume WiFi connection is already in place - done my Blynk_WiFiManager.)
* Use of EEPROM to remember the device labels in the app. (LittleFS was too large for SONOFF's relatively small memory.)
* Use of compiler constants for compile-time selection
* Use of Blynk's BLYNK_WRITE_DEFAULT() flexible capability
* Use of multiple devices on one Blynk Authcode

For each SONOFF device, the Blynk App wants to see...
* Blynk Button in SWITCH mode (ON/OFF)
* Blynk LED (Heartbeat)
* Blynk Value Display (Switch label)
* (optional) Blynk Button to force the WiFi Manager Config Portal to reset
             The DRD (Double Reset Detector) can do the same thing.

Arduino IDE requires 
* Arduino ESPcore installed (for the ESP8266-based SONOFFs)
* Blynk master library 0.6.x (not the 1.0.0-beta version) installed
* Blynk_WiFiManager library installed

You will also need, of course
* One or more SONOFF Basic (ESP8266) devices and the ability to flash the SONOFFs. (See elsewhere.)

As in all Blynk sketches using WiFi, you will need
* Your WiFi SSID or SSIDs and passwords
* Your Blynk authcode or authcodes
* Your Blynk server URL (I use blynk-cloud.com.)

These values do not need to be coded into the sketch as they may be entered at runtime (once) into the Config Portal.

The Config Portal will appear as SSID MyConfigPortal, with WiFi password of 12345678
and IP address of 192.168.220.1
These three values (Config Portal SSID, password, IP address) are hardcoded into the sketch
using #include files (but could be made selectable in the Config Portal, itself.)

See https://github.com/thorathome/Blynk_Examples
   
See also https://github.com/khoih-prog/Blynk_WM for the Blynk WiFi Manager library

