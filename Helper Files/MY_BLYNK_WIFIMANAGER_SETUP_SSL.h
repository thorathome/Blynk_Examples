// MY_BLYNK_WIFIMANAGER_SETUP
// #include "MY_BLYNK_WIFIMANAGER_SETUP.h" // in main code


// Config Portal WiFi is CONFIG_PORTAL_SSID, above. Password and IP address is here 
#define CONFIG_PORTAL_PASSWORD  "12345678"      // password for device-generated WiFi beacon - 8+ characters
#define CONFIG_PORTAL_IPADDRESS 192,168,220,1   // IP address of Config Portal once connected to WiFi beacon


///////////////////////////////////////////////////////////////////////////////////////////
//// COMPILER SWITCH SELECTION - USE DYNAMIC (CUSTOM) CONFIG PORTAL FIELDS OR NOT /////////
#define USE_DYNAMIC_PARAMETERS false // Not using Dynamic Parameters on this sketch (anymore)



///////////////////////////////////////////////////////////////////////////////////////////
//// COMPILER SWITCH SELECTION - USE DEFAULT CONFIG PORTAL FIELD DATA OR NOT //////////////
//// SELECT IF YOU WANT CONFIG PORTAL FIELDS INITIALIZED TO SOMETHING OTHER THAN BLANK ////
//// This data only gets loaded on an initial compile and upload and after Config Portal data gets deleted
//// such as by using DRD (Double Reset) or the Blynk command Blynk.clearConfigData()
#define USE_DEFAULT_CONFIG_DATA   true
//#define USE_DEFAULT_CONFIG_DATA false



///////////////////////////////////////////////////////////////////////////////////////////
//// COMPILER SWITCH SELECTION - USE LITTLEFS (ESP8266 ONLY), SPIFFS OR EEPROM /////////////////////////////////////
//// only relevant if using WiFiManager_WM  
//// https://github.com/khoih-prog/Blynk_WM/blob/master/examples/ESP8266WM_Config/defines.h

#define USE_SPIFFS        false   // Never ever use SPIFFS 

// Not use #define USE_LITTLEFS and #define USE_SPIFFS  => using SPIFFS for configuration data in WiFiManager
// (USE_LITTLEFS == false) and (USE_SPIFFS == false)    => using EEPROM for configuration data in WiFiManager
// (USE_LITTLEFS == true)  and (USE_SPIFFS == false)    => using LITTLEFS for configuration data in WiFiManager
// (USE_LITTLEFS == true)  and (USE_SPIFFS == true)     => using LITTLEFS for configuration data in WiFiManager
// (USE_LITTLEFS == false) and (USE_SPIFFS == true)     => using SPIFFS for configuration data in WiFiManager
// Those above #define's must be placed before #include <BlynkSimpleEsp32_WFM.h>

// #define USE_LITTLEFS        true  // MUST BE BEFORE #include

#if USE_LITTLEFS
  //LittleFS has higher priority
  #define CurrentFileFS     "LittleFS"
  #ifdef USE_SPIFFS
    #undef USE_SPIFFS
  #endif
  #define USE_SPIFFS        false
#elif USE_SPIFFS
  #define CurrentFileFS     "SPIFFS"
#else
  #define CurrentFileFS     "EEPROM"
#endif

// This piece here is ONLY for EEPROM implementation. Some differences between Khoih's ESP32 and 8266 code
#if !( USE_LITTLEFS || USE_SPIFFS) // ATTENTION: In Khoih's code ESP32 = 2048 and 2 * 1024, ESP8266 = 4096 and 4 * 1024
  // EEPROM_SIZE must be <= 4096 and >= CONFIG_DATA_SIZE (currently 172 bytes)
  #define EEPROM_SIZE    (4 * 1024)
  // EEPROM_START + CONFIG_DATA_SIZE must be <= EEPROM_SIZE
  #define EEPROM_START  768        // ATTENTION: In Khoih's code, ESP32 = 0, ESP8266 = 768
#endif





// Force some params in Blynk, only valid for library version 1.0.1 and later
#define TIMEOUT_RECONNECT_WIFI                    5000L
#define RESET_IF_CONFIG_TIMEOUT                   true
#define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET    2




// AUTOMATIC Blynk_WiFiManager LIBRARY SELECTION 
// This is BLYNK_WiFiManager library selection for ESP32 or 8266
#ifdef ESP8266
  #include <BlynkSimpleEsp8266_SSL_WM.h>
#endif
#ifdef ESP32
  #include <BlynkSimpleEsp32_SSL_WM.h> 
#endif





///////////////////////////////////////////////////////////////////////////////////////////
// Config Portal setup
#define CONFIG_PORTAL_PASSWORD  "12345678"     // password for device-generated WiFi beacon - 8+ characters
#define CONFIG_PORTAL_IPADDRESS 192,168,220,1 // IP address of Config Portal once connected to WiFi beacon

#if USE_DEFAULT_CONFIG_DATA // Set default values for "standard" fields presented in Config Portal
  bool LOAD_DEFAULT_CONFIG_DATA = true;  //do not modify - used by library
    
  ///////////////////////////////////////////////////////////////////////////////////////////
  //// COMPILER SWITCH SELECTIONS - SET DEFAULT VALUES PRESENTED IN CONFIG PORTAL ///////////////
  //// Data structure AND default values for "standard" fields presented in Config Portal
  Blynk_WM_Configuration defaultConfig =
  {
    // NOT NECESSARY TO MODIFY
    //char header[16], dummy, not used
      "SSL",

    ///////////////////////////////////////////////////////////////////////////////////////////
    //// COMPILER SWITCH SELECTIONS - SET UP TO TWO SSIDs & TWO passwords /////////////////////
    //WiFi_Credentials  WiFi_Creds  [NUM_WIFI_CREDENTIALS]
    //WiFi_Creds.wifi_ssid and WiFi_Creds.wifi_pw
    MY_WIFI_SSID,  MY_WIFI_PASSWORD,     // Config Portal WiFi SSID & PWD field values
    MY_WIFI_SSID2, MY_WIFI_PASSWORD2,    // Config Portal WiFi SSID1 & PWD1 field values
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    //// COMPILER SWITCH SELECTIONS - SET UP TO TWO BLYNK SERVERS & TWO AUTH CODES ////////////
    // Blynk_Credentials Blynk_Creds [NUM_BLYNK_CREDENTIALS];
    // Blynk_Creds.blynk_server and Blynk_Creds.blynk_token
    MY_BLYNK_SERVER, MY_BLYNK_AUTHCODE,  // Config Portal Blynk Server1 & Token1 field values
    MY_BLYNK_SERVER, MY_BLYNK_AUTHCODE,  // Config Portal Blynk Server1 & Token1 field values

    ///////////////////////////////////////////////////////////////////////////////////////////
    //// COMPILER SWITCH SELECTIONS - SET DEFAULT PORTS (FOR SSL OR NON-SSL) //////////////////
    //int  blynk_port;
    9443,                             // Config Portal Port field (default SSL port) value
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    //// COMPILER SWITCH SELECTIONS - SET DEFAULT BOARD NAME //////////////////////////////////
    //char board_name     [24];
      DEVICE_NAME, 
    
    // terminate the list
    //int  checkSum, dummy, not used
    0
    /////////// End Default Config Data /////////////
  };
   
#else // not using USE_DEFAULT_CONFIG_DATA 
  // Set up the "standard" Config Portal fields
  bool LOAD_DEFAULT_CONFIG_DATA = false;
    
  // AUTOMATICALLY GENERATE THE "STANDARD" CONFIG PORTAL DATA STRUCTURE
  // NOT NECESSARY TO MODIFY
  Blynk_WM_Configuration defaultConfig;  // loads the default config portal data type with blacnk defaults  
  
#endif



  
// We are NOT using Blynk_WiFiManager's USE_DYNAMIC_PARAMETERS - SET UP NULL (DYNAMIC) MENU (See doc)
// NOT NECESSARY TO MODIFY
MenuItem myMenuItems [] = {};
uint16_t NUM_MENU_ITEMS = 0;



