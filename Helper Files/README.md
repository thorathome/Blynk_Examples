I use these files to simplify repeated code in my sketches. 

To use
* Place them in one (or more) foldere(s) in your Arduino library folder. The Arduino IDE will find them when it starts up. 
* #include them in your code. I am working on better doc for these.  


* Several #include files placed in a folder in your library directory, downloaded for edit and use from this repository
* * MY_WIFI_CREDENTIALS.h - edit in your WiFi SSIDs and Passwords
* * MY_BLYNK_CREDENTIALS.h - edit in your Blynk Authcodes 
* * MY_BLYNK_WIFIMANAGER_SETUP_SSL.h - edit in you own preferences for standard Blynk_WiFiManager setup
* * MY_ESP_LED_BUILTINS.h - helpers for inverted ESP8266 LED logic
* * MY_BLYNK_COLORS.h - bunch of Blynk colors in hex
* * MY_SERIAL_PRINTS.h - helpers for Serial.print
* * OTABlynkTelNet.h or OTABlynk.h - Spiess' (slightly modified) OTA and OTA/TelNetStream code (See below for more info)


// ----------------------------  SAMPLE CODE --------------------------------
// OTA and TelNetStream code below from from Andreas Spiess on YouTube channel.  
#define USE_OTA true
//define USE_OTA false

#if USE_OTA
  #define USE_TELNET true
  //#define USE_TELNET false
#endif

/* this controls library loads, the recurring call in <loop> and TelNetStream output */
#if USE_OTA && USE_TELNET
  #include "OTABlynkTelNet.h"      // stub from Andreas Spiess in my own directory
#elif USE_OTA && ! USE_TELNET
  #include "OTABlynk.h"            // stub from Andreas Spiess in my own directory
#endif




#define MY_SERIAL_PRINT_ON true    // All mySerialPxxx to work as written
//#define MY_SERIAL_PRINT_ON false   // NO Serial.printing at all

#if MY_SERIAL_PRINT_ON
  #define BLYNK_PRINT       Serial // Generates Blynk debug prints. Comment out if not needed, saving space 
  #define BLYNK_WM_DEBUG    0      // 0 - 3
#endif
#include "MY_SERIAL_PRINTS.h"      // #defines mySerialP, mySerialPln, mySerialPfx, mySeriamBegin for mySerialP & ...ln if MY_SERIAL_PRINT is true, No-op if false




// My shortcut libraries
#include "MY_WIFI_CREDENTIALS.h"   // #defines MY_WIFI_SSID/2 AND MY_WIFI_PASSWORD/2
#include "MY_BLYNK_CREDENTIALS.h"  // #defines MY_BLYNK_SERVER and MY_xxx_AUTHCODE
#include "MY_ESP_LED_BUILTINS.h"   // #defines LED_BUILTIN_HIGH and LOW for uniform use across ESP32 and ESP8266 devices
#include "MY_BLYNK_COLORS.h"       // #defines a bunch of handy Blynk colors

