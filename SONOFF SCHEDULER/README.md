**THIS IS WIP. LOADED SKETCH WORKS, STILL NEEDS POLIOSHING  **

The Arduino/Blynk sketch SONOFF_SCHEDULER.ino is an **OTA-enabled, multi-device SONOFF Scheduling system** using the powerful BlynkSimpleESP8266_SSL_WM WiFiManager (WM) libraries for ESP8266. 

[SONOFF directory contains the Arduino Blynk OTA code. Helper Files directory has required #include files.]

**This sketch Schedules multiple SONOFFs controlled by the sketch SONOFF_OTA…** This sketch also uses advanced Blynk and Arduino IDE features.  It runs on ESP32 and ESP8266, AND is noticeably faster on ESP32. 

This sketch demonstrates several Blynk, Blynk_WiFiManager, and Arduino IDE features.
* Basic Blynk_WiFiManager Config Portal configuration and use
* Wonderful OTA code from Andreas Spiess, The Guy With the Swiss Accent, from his YouTube video #332  _(I modified his code a bit to assume WiFi connection is already in place - set up by Blynk_WiFiManager. Modified code available in this repository. Thank you, Andreas Spiess.)_
* Use of Blynk's BLYNK_WRITE_DEFAULT() flexible capability (to handle multiple Schedules)
* Use of Blynk’s Bridge capability to ‘talk to’ the SONOFFs. (The Scheduler does not 'listen' to the SONOFFs, though!)
* Use of compiler constants for compile-time selection of
* * Number of SONOFFs to Schedule (currently set at 6)
* * Number of independent Schedules (currently set at 3)
* Use of LittleFS to remember the Schedule information in flash memory. 

For each Schedule, the Blynk App wants to see...
* TWO Blynk Time Input Widgets, one for entering schedule info, the other for displaying the week's schedule
* ONE for EACH SONOFF Blynk Styled Button to Include/not-include that SONOFF in this Schedule
* (Optional) Blynk Text Input Widget (force a Reboot)
* (Virtual Pins are hardcoded into the sketch) 
* (Assumes all the SONOFFs are on one Blynk Authcode)
* (Virtual Pins hardcoded in the sketch using #compiler directives)
* **(THEREFORE, for 3 Schedules and 6 SONOFFs, you'll want 6 Time Input Widgets and 18 Styled Buttons.)**

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
* the SONOFF control program running. 

As in all Blynk sketches using WiFi, you will need
* Your WiFi SSID or SSIDs and passwords
* Your Blynk authcode or authcodes
* Your Blynk server URL (I use blynk-cloud.com.)

The Blynk_WiFiManager has a Config Portal for entering WiFi and Blynk Server info at runtime. However, I hardcode this into the sketch, and can change it using the Config Portal if I ever need to. 

The Config Portal will appear as SSID MyConfigPortal, with WiFi password of 12345678
and IP address of 192.168.220.1  
These values (Config Portal SSID, password, IP address) are hardcoded into the sketch
using #include files. 

This repository https://github.com/thorathome/Blynk_Examples 
   
See also https://github.com/khoih-prog/Blynk_WM for the Blynk WiFi Manager library. (You can download it directly from the IDE.)

See Andreas Spiess on YouTube for his OTA episodes. This one refers to ESP32, but is also applicable to ESP8266/SONOFF.
https://youtu.be/1pwqS_NUG7Q

