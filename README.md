/*

   The Arduino/Blynk sketch SONOFF BLYNK OTA.ino is a fully-developed SONOFF control system using
   the powerful BlynkSimpleESP8266_SSL_WM WiFiManager (WM) libraries for ESP8266.
   
   See https://github.com/thorathome/Blynk_Examples
   
   See also https://github.com/khoih-prog/Blynk_WM for the Blynk WiFi Manager library

   It demonstrates
 * * WiFiManager Config Portal configuration and use
 * * Wonderful OTA code from Andreas Spiess, The Guy With the Swiss Accent, on his YouTube video #332
 * * (I modified his code a bit to assume WiFi connection is already in place - done my Blynk_WiFiManager.)
 * * Use of EEPROM to remember the device labels int he app. (LittleFS was too large for SONOFF memory.)*
 * * Use of compiler constants for compile-time selection
 * * Use of Blynk's BLYNK_WRITE_DEFAULT() flexible capability
 * * Use of multiple devices on one Blynk Authcode

   It requires:
 * * Blynk Button in SWITCH mode
 * * Blynk LED
 * * Blynk Value Display
 * * (optional) Blynk Button to force the WiFi Manager Config Portal to reset
                The DRD (Double Reset Detector) can do the same thing.
 * * Blynk master library installed
 * * Blynk_WiFiManager installed
 * * SONOFF Basic (ESP8266) device and the ability to flash it. (See elsewhere.)

   As in all Blynk sketches using WiFi, you will need
 * * Your WiFi SSID or SSIDs and passwords
 * * Your Blynk authcode or authcodes
 * * Your Blynk server URL (Main US Blynk server is blynk-cloud.com.)

   These values do not need to be coded into the sketch as they may be entered at runtime (once)
   into the Config Portal.

   The Config Portal will appear as SSID MyConfigPortal, with WiFi password of 12345678
   and IP address of 192.168.220.1
   These three values (Config Portal SSID, password, IP address) are hardcoded into the sketch
   using #include files (but could be made selectable in the Config Portal, itself.)
*/
