# Blynk WiFi & Color Templates and Helper Examples

## Blynk_WiFiManager template and Color selector example
Updated 27 June 2020 to Blynk_WiFiManager (WM) library update 1.0.16

The Arduino/Blynk sketch **Blynk_WM_Template.ino** is a fully-developed get-started demo program for the powerful BlynkSimpleESP... and the newer WiFiManager (WM) libraries.**It is also a Blynk HEX color selector.**

It demonstrates 
 * **WiFiManager Config Portal** configuration and use
 * **WiFiManager Dynamic (extended) Parameters** configuration and use
 * **LittleFS file system for ESP8266** is an option along with EEPROM and the soon-to-be deprecated SPIFFS
 * Use of **compiler constants** for compile-time selection
 * Use of Blynk's **BLYNK_WRITE_DEFAULT()** flexible capability
 
This sketch lets end-users choose a color using ZeRGBa, colors a blinking (heartbeat) LED, and shows the HEX code for that color.

It requires:
 * Blynk ZeRGBa configured in MERGE mode with values between 0 and 255
 * Blynk LED
 * Blynk Value Display
 * Blynk master library installed
 * Blynk_WiFiManager library installed
 * ESP32 or ESP8266 devices
 
As in all Blynk sketches using WiFi, you will need
 * Your WiFi SSID or SSIDs and passwords
 * Your Blynk authcode or authcodes
 * Your Blynk server URL (Main US Blynk server is blynk-cloud.com.)
 
These values do not need to be coded into the sketch as they may be entered at runtime (once) into the Config Portal.
 
You also do not have to hardcode the Virtual Pins for the ZeRGBa, LED or Value Display, can input them at runtime using the Config Portal.
This sketch uses Blynk's BLYNK_WRITE_DEFAULT() capability to deliver this flexibility. I use this approach to manage a number of similar wireless controls like SONOFFs with the same sketch.  
 
The Config Portal will appear as SSID MyConfigPortal, with WiFi password of 12345678 and IP address of 192.168.220.1. 
These three values (Config Portal SSID, password, IP address) are hardcoded into the sketch (but could be made selectable in the Config Portal, itself.)
 
I hope this is as useful to you as it has been to me to understand Blynk, the BlynkSimpleESP... and ...WiFiManager libraries, the ESP32 and ESP8266.  https://github.com/thorathome/Blynk_Examples
 
 **Thanks to @khoih-prog (@khoih on Blynk Community) for updating this example to include LittleFS and for writing the powerful WiFiManager library**



## MY_WIFI_CREDENTIALS.h
Quick and easy way to get my personal WiFi credentials out of my code.  
Download and look at the file for documentation. 

Short story: (**Thanks to Andreas Spiess on YouTube for this hint!**)
* Create a folder called MY_WIFI_CREDENTIALS inside your libraries folder .  
* Download MY_WIFI_CREDENTIALS.h into the MY_WIFI_CREDENTIALS folder.
* Edit the file to show your own WiFi credentials.
* Use **#include "MY_WIFI_CREDENTIALS.h"** in your sketch.  
* Use **MY_WIFI_SSID** and **MY_WIFI_PASSWORD** in your sketches instead of your actual SSID and WiFi password. 


## MY_BLYNK_CREDENTIALS.h
Quick and easy way to get my personal Blynk credentials out of my code.  
Download and look at the file for documentation. 

Short story: (**Thanks to Andreas Spiess on YouTube for this hint!**)
* Create a folder called MY_BLYNK_CREDENTIALS inside your libraries folder.  
* Download MY_BLYNK_CREDENTIALS.h into the MY_BLYNK_CREDENTIALS folder.
* Edit the file to show your own Blynk credentials.
* Use descriptive names in the #define statements, names you can remember and use in your sketches.
* Use **#include "MY_BLYNK_CREDENTIALS.h"** in your sketch.  
* Do not forget to **#define MY_BLYNK_AUTHCODE whatever-descriptive-name-you-need** in your code.
* Use **MY_BLYNK_SERVER** and **MY_BLYNK_AUTHCODE** in your sketches instead of your actual Blynk info. 


## MY_BLYNK_COLORS.h
I've seen on Blynk many questions and confusions about HEX color codes. Here is a small set of helpful colors I often use.  I'll expand these as I create more. 

To use easily (**Thanks to Andreas Spiess on YouTube for this hint!**)
* Create a folder called MY_BLYNK_COLORS inside your libraries folder .  
* Download MY_BLYNK_COLORS.h into the MY_BLYNK_COLORS folder.
* Use **#include "MY_BLYNK_COLORS.h"** in your sketch to get these color samples easily.  


## ESP_LED_BUILTINS.h
When I found out that the Arduino ESP8266 libraries have the LED_BUILTIN inverted (HIGH is LOW and LOW is HIGH - ugh.)
I built this little code snippet. 

So place this file in your Arduino/libraries folder and put **#include "ESP_LED_BUILTINS.h"** in your sketch.
Use LED_BUILTIN_HIGH for HIGH and LED_BUILTIN_LOW for LOW and it will work with either ESP platform.  

Also, I found that some ESP32s do not have a LED-BUILTIN defined in the Arduino board library. LED_BUILTIN is 13 for ESP32.



All comments welcome....
