// This is a simple Kludge that lets sketches be more device-independent
// Arduino for ESP8266 inverts the HIGH/LOW for the onboard LED_BUILTIN
// Use LED_BUILTIN_HIGH and LED_BUILTIN_LOW in your sketch and all will be right with the world

// #include "ESP_LED_BUILTINS.h"      // #defines LED_BUILTIN_HIGH and /LOW for uniform use across devices

#ifdef ESP8266  // There's a quirk in Arduino that has the LED_BUILTIN inverted. This corrects for it.
  #define LED_BUILTIN_HIGH LOW
  #define LED_BUILTIN_LOW HIGH
#else
  #define LED_BUILTIN_HIGH HIGH
  #define LED_BUILTIN_LOW LOW
#endif

// Also the ESP32 board manager files appear to NOT have LED_BUILTIN assigned correctly
// This fixes that
#ifdef ESP32
  #define LED_BUILTIN 13  // NOT DEFINED IN ESP32 BOARD FILES - HMMM.  
#endif
