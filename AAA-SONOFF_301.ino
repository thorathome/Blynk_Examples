#define SKETCH_NAME "SONOFF 301"
#define SERIAL_SPEED 230400

#ifndef ESP8266
  #error Hey this is for Sonoff and ESP8266
#endif

#define BLYNK_PRINT       Serial  // Generates Blynk debug prints. Comment out if not needed, saving space
#define BLYNK_WM_DEBUG    0

#include "MY_WIFI_CREDENTIALS.h"   // #defines MY_WIFI_SSID AND MY_WIFI_PASSWORD
#include "MY_BLYNK_CREDENTIALS.h"  // #defines MY_BLYNK_SERVER and MY_xxx_AUTHCODE
#include "ESP_LED_BUILTINS.h"      // #defines LED_BUILTIN_HIGH and _LOW for uniform use across devices
#include "MY_BLYNK_COLORS.h"       // #defines a bunch of handy Blynk colors

//#define REAL_SONOFF true   // Sets device name, onboard LED hardware pin and the relay checker
#define REAL_SONOFF false   // Sets device name, onboard LED hardware pin and the relay checker

#if REAL_SONOFF
  #define MY_BLYNK_AUTHCODE MY_REAL_SONOFF_AUTHCODE 
  #define SONOFF_DEVICE_NAME "CURLY" // "LARRY"  "CURLY"  "MOE" 
#else
  #define MY_BLYNK_AUTHCODE MY_WORKING_SONOFF_AUTHCODE   
  #define SONOFF_DEVICE_NAME "SONOFF-DEV"
#endif

// SONOFF ESP8266 HARDWARE PINS
#if REAL_SONOFF
  #define SONOFF_RELAY_PIN  12 // SONOFF RELAY ON/OFF
  #define LED_BUILTIN 13       // SONOFF ONBOARD LED
#else
  #define SONOFF_RELAY_PIN  5  // SONOFF RELAY ON/OFF
#endif 

// Cutting down on the brightness of the Blynk LED Widget
#define BLYNK_LED_WIDGET_MAX_BRIGHTNESS 150 // Led Widget is [0-255]  150 = not very bright (from 255)


/*  
 * The Arduino/Blynk sketch SONOFF 101.ino is a fully-developed SONOFF control system using 
 * the powerful BlynkSimpleEsp...  WiFiManager (WM) libraries  for ESP8266. 
 * See https://github.com/thorathome/Blynk_Examples
 * 
 * It demonstrates
 * * WiFiManager Config Portal configuration and use
 * * WiFiManager Dynamic (extended) Parameters configuration and use
 * * Use of compiler constants for compile-time selection
 * * Use of Blynk's BLYNK_WRITE_DEFAULT() flexible capability
 *
 * This sketch creates a simple Blynk on/off switch, an indicator Blynk LED and a display panel 
 * to tell me what this is controlling
 * It lets me choose at runtime (without recompile or download to SONOFF - kind of a pain) the
 * * SONOFF device name
 * * Blynk Virtual Pins I will use for this particular device
 *
  * It requires:
 * * Blynk Button in SWITCH mode
 * * Blynk LED
 * * Blynk Value Display
 * * Blynk master library installed
 * * Blynk_WiFiManager installed
 * * SONOFF Basic (ESP8266) device and the ability to flash it. (See elsewhere.)  
 *
 * As in all Blynk sketches using WiFi, you will need
 * * Your WiFi SSID or SSIDs and passwords
 * * Your Blynk authcode or authcodes
 * * Your Blynk server URL (Main US Blyk server is blynk-cloud.com.)
 *
 * These values do not need to be coded into the sketch as they may be entered at runtime (once) 
 * into the Config Portal.
 *
 * You also do not have to hardcode the Virtual Pins for the Blynk Button, LED or Value Display, 
 * can input them at runtime using the Config Portal. 
 * This sketch uses Blynk's BLYNK_WRITE_DEFAULT() capability to deliver this flexibility. 
 * I use this approach to manage a number of similar wireless controls like SONOFFs.
 *
 * The Config Portal will appear as SSID MyConfigPortal, with WiFi password of 12345678 
 * and IP address of 192.168.220.1 
 * These three values (Config Portal SSID, password, IP address) are hardcoded into the sketch 
 * (but could be made selectable in the Config Portal, itself.)
 *
 */


///////////////////////////////////////////////////////////////////////////////////////////
//// COMPILER SWITCH SELECTION - USE WIFI MANAGER OR NOT //////////////////////////////////
#define USE_WM true   // to easily select WiFi Manager or not
//#define USE_WM false   // to easily select WiFi Manager or not
// REMEMBER: not using _WM means we make our own initial WiFi connection


///////////////////////////////////////////////////////////////////////////////////////////
//// COMPILER SWITCH SELECTION - USE SSL OR NOT ///////////////////////////////////////////
#define USE_SSL true // to easily select SSL or not
//#define USE_SSL false // to easily select SSL or not


///////////////////////////////////////////////////////////////////////////////////////////
//// COMPILER VALUE SELECTION - VIRTUAL PINS FOR THE 3 WIDGETS ////////////////////////////
// Larry is 20-23, Curly is 30-33, Moe is 40-43
#define CONTROL_DEFAULT_VPIN       "40"  // Can also be changed via Config Portal (USE_DYNAMIC_PARAMETERS)
#define HEARTBEAT_LED_DEFAULT_VPIN "41"  // Can also be changed via Config Portal (USE_DYNAMIC_PARAMETERS)
#define DISPLAY_DEFAULT_VPIN       "42"  // Can also be changed via Config Portal (USE_DYNAMIC_PARAMETERS)
#define RESET_DEFAULT_VPIN         "43"  // "Invisible" widget to clear the Config Portal values
// These can all be reset using Config Portal. I have included them as default values only. */ 


///////////////////////////////////////////////////////////////////////////////////////////
//// COMPILER SWITCH SELECTION - USE DEFAULT CONFIG PORTAL FIELD DATA OR NOT //////////////
//// SELECT IF YOU WANT CONFIG PORTAL FIELDS INITIALIZED TO SOMETHING OTHER THAN BLANK ////
//// This data only gets loaded on an initial compile and upload and after Config Portal data gets deleted
//// such as by using DRD (Double Reset) or the Blynk command Blynk.clearConfigData()
#define USE_DEFAULT_CONFIG_DATA true  
//#define USE_DEFAULT_CONFIG_DATA false


///////////////////////////////////////////////////////////////////////////////////////////
//// COMPILER SWITCH SELECTION - USE DYNAMIC (CUSTOM) CONFIG PORTAL FIELDS OR NOT /////////
#define USE_DYNAMIC_PARAMETERS true  // We want this to enter WiFi info, names and Vpins at runtime
//#define USE_DYNAMIC_PARAMETERS false

  
///////////////////////////////////////////////////////////////////////////////////////////
//// COMPILER SWITCH SELECTION - USE SPIFFS OR EEPROM /////////////////////////////////////
//// only relevant if using WiFiManager _WM
#define USE_SPIFFS false  // Choosing EEPROM over SPIFFS here
//#define USE_SPIFFS true // Have found SPIFFS take more compile time than EEPROM


// COMPILE-TIME LOGIC: NON-VOLATILE MEMORY SELECTION (WiFiManager only) 
// NOT NECESSARY TO MODIFY
#if (!USE_SPIFFS)
  // EEPROM_SIZE must be <= 2048 and >= CONFIG_DATA_SIZE (currently 172 bytes)
  #define EEPROM_SIZE    (2 * 1024)
  // EEPROM_START + CONFIG_DATA_SIZE must be <= EEPROM_SIZE
  #define EEPROM_START   512
#endif


// NOT NECESSARY TO MODIFY - MUST BE INCLUDED
// Force some params in Blynk, only valid for library version 1.0.1 and later
// (from the Github doc)
#define RESET_IF_CONFIG_TIMEOUT                   true
#define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET    3  // Library default is 10 (times 2) - DEBUG SET AT 2


// NOT NECESSARY TO MODIFY
// COMPILE-TIME LOGIC: AUTOMATIC LIBRARY SELECTION 
#if USE_SSL
  #include <BlynkSimpleEsp8266_SSL_WM.h>
#else // no SSL
  #include <BlynkSimpleEsp8266_WM.h>
#endif


#if USE_WM
  ///////////////////////////////////////////////////////////////////////////////////////////
  //// COMPILER VALUE SELECTION - CONFIG PORTAL'S OWN SSID AND password /////////////////////
  //// Config Portal turns on when WiFiManager cannot connect to WiFi or Blynk
  //// only relevant if using WiFiManager _WM
  #define CONFIG_PORTAL_SSID      "ConfigSONOFF" // SSID for device-generated WiFi beacon
  #define CONFIG_PORTAL_PASSWORD  "12345678"     // password for device-generated WiFi beacon - 8+ characters
  #define CONFIG_PORTAL_IPADDRESS 192,168,220,1  // IP address of Config Portal once connected to WiFi beacon


  /////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////
  #define DEVICE_HOST_NAME SONOFF_DEVICE_NAME  // Shows up in Config Portal as Tab Name
                                               // ALSO shows up in Router table as device name
  // What's the difference between this and Board Name ??
  /////////////////////////////////////////////////////////////////////


  #if USE_DEFAULT_CONFIG_DATA // Set default values for "standard" fields presented in Config Portal
    bool LOAD_DEFAULT_CONFIG_DATA = true;  //do not modify - used by library
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    //// COMPILER SWITCH SELECTIONS - SET DEFAULT VALUES PRESENTED IN CONFIG PORTAL ///////////////
    //// Data structure AND default values for "standard" fields presented in Config Portal
    Blynk_WM_Configuration defaultConfig =
    {
      // NOT NECESSARY TO MODIFY
      //char header[16], dummy, not used
      #if USE_SSL  
        "SSL",
      #else
        "NonSSL",
      #endif

      ///////////////////////////////////////////////////////////////////////////////////////////
      //// COMPILER SWITCH SELECTIONS - SET UP TO TWO SSIDs & TWO passwords /////////////////////
      //WiFi_Credentials  WiFi_Creds  [NUM_WIFI_CREDENTIALS]
      //WiFi_Creds.wifi_ssid and WiFi_Creds.wifi_pw
      MY_WIFI_SSID, MY_WIFI_PASSWORD,              // Config Portal WiFi SSID & PWD field values
      MY_WIFI_SSID, MY_WIFI_PASSWORD,              // Config Portal WiFi SSID & PWD field values
    
      ///////////////////////////////////////////////////////////////////////////////////////////
      //// COMPILER SWITCH SELECTIONS - SET UP TO TWO BLYNK SERVERS & TWO AUTH CODES ////////////
      // Blynk_Credentials Blynk_Creds [NUM_BLYNK_CREDENTIALS];
      // Blynk_Creds.blynk_server and Blynk_Creds.blynk_token
      MY_BLYNK_SERVER, MY_BLYNK_AUTHCODE,  // NEW SONOFF Three Panel
      MY_BLYNK_SERVER, MY_BLYNK_AUTHCODE,  // NEW SONOFF Three Panel

      ///////////////////////////////////////////////////////////////////////////////////////////
      //// COMPILER SWITCH SELECTIONS - SET DEFAULT PORTS (FOR SSL OR NON-SSL) //////////////////
      //int  blynk_port;
      #if USE_SSL
        9443,                           // Config Portal Port field (default SSL port) value
      #else
        8080,                           // Config Portal Port field (default non-SSL) value
      #endif
    
      ///////////////////////////////////////////////////////////////////////////////////////////
      //// COMPILER SWITCH SELECTIONS - SET DEFAULT BOARD NAME //////////////////////////////////
      //char board_name     [24];
      "SONOFF_1-2-3",                    // Config Portal Board Name field value
    
      // terminate the list
      //int  checkSum, dummy, not used
      0
      /////////// End Default Config Data /////////////
    };
   
  #else // not using USE_DEFAULT_CONFIG_DATA 
    // Set up the "standard" Config Portal fields
    bool LOAD_DEFAULT_CONFIG_DATA = false;
    
    // AUTOMATICALLY GENERATE THE "STANDARD"CONFIG PORTAL DATA STRUCTURE
    // NOT NECESSARY TO MODIFY
    Blynk_WM_Configuration defaultConfig;  // loads t4he default confir portal data type with blacnk defaults  
  #endif

  
  #if USE_DYNAMIC_PARAMETERS
    ///////////////////////////////////////////////////////////////////////////////////////////
    //// COMPILER VALUE SELECTION - SET UP DYNAMIC PARAMETER DATA TYPES & INITIAL VALUES //////
    //Defined in <BlynkSimpleEsp8266_WM.h> and <BlynkSimpleEsp8266_SSL_WM.h>   
    /**************************************
    #define MAX_ID_LEN                5
    #define MAX_DISPLAY_NAME_LEN      16

    typedef struct
    {
    char id             [MAX_ID_LEN + 1];
    char displayName    [MAX_DISPLAY_NAME_LEN + 1];
    char *pdata;
    uint8_t maxlen;
    } MenuItem;
    ************************************* */    
    ///////////////////////////////////////////////////////////////////////////////////////////
    //// COMPILER VALUE SELECTION - SET UP FIELD LENGTHS AND DEFAULT VALUES FOR EACH DYNAMIC PARAMETER
    #define CONTROL_VPIN_LENGTH 5 // First DYNAMIC_PARAMETER (Control - ON/OFF Virtual Pin)
    char controlVpinC [ CONTROL_VPIN_LENGTH + 1 ] = CONTROL_DEFAULT_VPIN;

    ///////////////////////////////////////////////////////////////////////////////////////////
    //// COMPILER VALUE SELECTION - SET UP FIELD LENGTHS AND DEFAULT VALUES FOR EACH DYNAMIC PARAMETER
    #define CONTROL_LABEL_LENGTH 18 // Second DYNAMIC_PARAMETER (Widget Label for Control) 
    char controlLabel [ CONTROL_LABEL_LENGTH + 1 ] = SONOFF_DEVICE_NAME;

    ///////////////////////////////////////////////////////////////////////////////////////////
    //// COMPILER VALUE SELECTION - SET UP FIELD LENGTHS AND DEFAULT VALUES FOR EACH DYNAMIC PARAMETER
    #define HEARTBEAT_LED_VPIN_LENGTH 5 // Third DYNAMIC_PARAMETER (Heartbeat LED Virtual Pin)
    char heartbeatVpinC [ HEARTBEAT_LED_VPIN_LENGTH + 1 ] = HEARTBEAT_LED_DEFAULT_VPIN;  

    ///////////////////////////////////////////////////////////////////////////////////////////
    //// COMPILER VALUE SELECTION - SET UP FIELD LENGTHS AND DEFAULT VALUES FOR EACH DYNAMIC PARAMETER
    #define HEARTBEAT_LED_LABEL_LENGTH 26 // Fourth DYNAMIC_PARAMETER (Widget Label for Heartbeat LED)
    char heartbeatLabel [ HEARTBEAT_LED_LABEL_LENGTH + 1 ] = "Working";  

    ///////////////////////////////////////////////////////////////////////////////////////////
    //// COMPILER VALUE SELECTION - SET UP FIELD LENGTHS AND DEFAULT VALUES FOR EACH DYNAMIC PARAMETER
    #define DISPLAY_VPIN_LENGTH 5 // Fifth DYNAMIC_PARAMETER (Display Virtual Pin)
    char displayVpinC [ DISPLAY_VPIN_LENGTH + 1 ] = DISPLAY_DEFAULT_VPIN;  

    ///////////////////////////////////////////////////////////////////////////////////////////
    //// COMPILER VALUE SELECTION - SET UP FIELD LENGTHS AND DEFAULT VALUES FOR EACH DYNAMIC PARAMETER
    #define DISPLAY_LABEL_LENGTH 26 // Sixth DYNAMIC_PARAMETER (Widget Label for Display)
    char displayLabel [ DISPLAY_LABEL_LENGTH + 1 ] = SONOFF_DEVICE_NAME;  


    ///////////////////////////////////////////////////////////////////////////////////////////
    //// COMPILER VALUE SELECTION - SET THE LABELS SHOWN IN CONFIG PORTAL FOR EACH DYNAMIC VARIABLE
    //// MAKE SURE YOUR DYNAMIC_PARAMETERS MATCH THIS STRUCTURE ////    
    MenuItem myMenuItems [] =
    { // 1234    123456789012345
      { "cvpn", "Button VPin(#)",  controlVpinC,    CONTROL_VPIN_LENGTH },
      { "clab", "Button Label",    controlLabel,    CONTROL_LABEL_LENGTH },
      { "hvpn", "Hbeat VPin(#)",   heartbeatVpinC,  HEARTBEAT_LED_VPIN_LENGTH },
      { "hlab", "Hbeat Label",     heartbeatLabel,  HEARTBEAT_LED_LABEL_LENGTH },
      { "dvpn", "Display VPin(#)", displayVpinC,    DISPLAY_VPIN_LENGTH },
      { "dlab", "Display Label",   displayLabel,    DISPLAY_LABEL_LENGTH },
    };

    // Automatically calculate the # of DYNAMIC PARAMETERS menu items - DO NOT TOUCH
    uint16_t NUM_MENU_ITEMS = sizeof ( myMenuItems ) / sizeof ( MenuItem );  //MenuItemSize;

  #else // not using USE_DYNAMIC_PARAMETERS - SET UP NULL (DYNAMIC) MENU
    #error Must use WM in this sketch
  #endif
#endif


// Integer versions of our three Virtual Pins - values loaded in Setup
int controlVpin;   // = CONTROL_DEFAULT_VPIN or set in Config Portam (WM)
int heartbeatVpin; // = HEARTBEAT_LED_DEFAULT_VPIN or set in Config Portam (WM)
int displayVpin;   // = DISPLAY_DEFAULT_VPIN or set in Config Portam (WM)
int resetVpin;     // = RESET_DEFAULT_VPIN or set to MAX+1 of the previous 3





// THIS SKETCH SETS UP A HEARTBEAT LED ON A TIMER TO SHOW SYSTEM IS ALIVEAND WELL
BlynkTimer myTimer;
// Blynk timers to blink a heartbeat LED on and off
int heartbeatLEDinterval = 5000; // interval between heartbeats for onboard and Blynk Virtual LED in millisec 
int heartbeatLEDtimerID; 

int heartbeatLEDduration = 750; // duraction of each blink in millisec (set as an interval timer)
int heartbeatLEDdurationTimerID;
bool heartbeatLEDon = false; // this lets me use the same routine for the turn-on timer and the turn-off interval


// THIS TIMER IGNORES (AND RESTORES) BUTTON PUSHES WITHIN A SECOND OR SO
int ignoreQuickButtonPushesTimerDuration = 900;  // ignore quick button pushes for a second or so
int ignoreQuickButtonPushesTimerID;  
bool ignoreQuickButtonPushes = false;  


// AND ANOTHER QUICKIE TIMER TO CHECK IF THE RELAY ACTUALLY SWITCHED AFTER A BUTTON PUSH
int verifyRelayTimerDuration = 995;  // verify the relay switched 1 second after button push
int verifyRelayTimerID;


// There's a bug in Blynk that allows all sorts of junk BLYNK_WRITE calls immediately on connecting to the app.
// This timer runs once upon connection and causes all new BLYNK_WRITEs to be ignored for a second or so
int flushAtStartupTimerDuration = 750; // 750 millisecs to flush everything out.  
int flushAtStartupTimerID;  
bool flushAtStartup = true; // FLUSH all BLYNK_WRITEs upon startup (Blynk Bug)


// This program is a state machine
// Pushbutton hits and checkRelay() can change the state
// updateBlynkWidgets paints the screen based on the state
#define BUTTON_OFF_RELAY_OFF          1 // Start with Button Off and Relay Off
#define BUTTON_ON_WAITING_FOR_RELAY   2 // Button goes ON, Relay still Off
#define BUTTON_ON_RELAY_ON            3 // Button still On, Relay went On
#define BUTTON_OFF_WAITING_FOR_RELAY  4 // Button goes Off, Relay still On
#define INITIALIZING                  5 // Just getting started
#define SYSTEM_ERROR                  6 // ALL SCREWED UP
int systemState = INITIALIZING; 

// Basic States of the ON/Off Buttoin and the SONOFF Relay
bool controlButtonON = false;   // The initial state of the Widget Button
bool relayON =         false;   // The initial state of the Relay
bool switchingRelay =  false;   // Is the relay being switched right now? 





// SETUP WIFI, BLYNK, HEARTBEAT
 void setup() 
{
  // HELP DEBUG THIS SKETCH WITH GOOD PRINTOUTS
  Serial.begin ( SERIAL_SPEED );
  delay ( 500 );  
  Serial.println ( "\n\n=======================================" );
  Serial.print ( SKETCH_NAME ); 
  #if USE_SSL
    Serial.print ( " ** Using SSL **" );  
  #endif
  Serial.println();


  pinMode ( SONOFF_RELAY_PIN, OUTPUT );  
  digitalWrite ( SONOFF_RELAY_PIN, LOW ); // turn that sucker OFF asap 

  // Initialize Onboard LED - turn ON during startup
  pinMode ( LED_BUILTIN, OUTPUT );  
  digitalWrite ( LED_BUILTIN, LED_BUILTIN_HIGH ); 

  // There's a Blynk bug that causes all sorts of erroreous BLYNK_WRITEs immediately upon connect
  // This kills all new BLYNK_WRITEs for a second
  flushAtStartup = true;  // Flush everthing BLYNK_WRITE_DEFAULT sees for a second or so at startup
  
  systemState = INITIALIZING;  
  connectToWLANandBlynk();  // Connect to WiFi, then to Blynk server

  setupBlynkTimers();  // Establish Blynk timers after Blynk is connected

  // Make sure the relay is off (turnSONOFFoff uses a Blynk timer)
  turnSONOFFoff();  
  
  // Set Blynk Virtual Heartbeat and LED_BUILTIN OFF
  digitalWrite ( LED_BUILTIN, LED_BUILTIN_LOW ); 
  Blynk.virtualWrite ( heartbeatVpin, 000 );  
  
  heartbeatLEDon = false;  
  heartbeatLEDblink(); // start first heartbeat (uses timers, too)
  
  Serial.println ( "\nSetup complete \n*******************************\n" );    
  
} //end setup





// KEEPING IT SIMPLE
void loop()
{
  Blynk.run();
  myTimer.run();  
  
} // end loop





// CHECK TO SEE IF THE RELAY STATE MATCHES THE CONTROL
void checkRelay()
{
  Serial.print ( "\n    checkRelay called: " ); 
    
  #if REAL_SONOFF
    if ( digitalRead ( SONOFF_RELAY_PIN ) == 1 )
      relayON = true;
    else
      relayON = false;  
  #else
    relayON = controlButtonON;  // Match the button while developing without an actual SONOFF
  #endif

  Serial.print ( ", relayON = " ); Serial.print ( relayON );  
  Serial.print ( ", controlButtonON = " ); Serial.println ( controlButtonON ); 

  Serial.print ( "        Current systemState = " ); Serial.println ( systemState ); 
 
  if ( relayON )
  {
    switch ( systemState )
    {
      case BUTTON_OFF_RELAY_OFF:
      {
        systemState = SYSTEM_ERROR;  
        break;
      }

      case BUTTON_ON_WAITING_FOR_RELAY:
      {
        systemState = BUTTON_ON_RELAY_ON;  
        break;
      }
    
      case BUTTON_ON_RELAY_ON:
      {
        systemState = BUTTON_ON_RELAY_ON;  
        break;
      }
    
      case BUTTON_OFF_WAITING_FOR_RELAY:
      {
        systemState = BUTTON_OFF_WAITING_FOR_RELAY;  
        break;
      }
    
      case INITIALIZING:
      {
        systemState = BUTTON_OFF_WAITING_FOR_RELAY;  
        break;
      }
    }
  }
  else // relay is OFF
  {
    switch ( systemState )
    {
      case BUTTON_OFF_RELAY_OFF:
      {
        systemState = BUTTON_OFF_RELAY_OFF;  
        break;
      }
    
      case BUTTON_ON_WAITING_FOR_RELAY:
      {
        systemState = BUTTON_ON_WAITING_FOR_RELAY;  
        break;
      }
    
      case BUTTON_ON_RELAY_ON:
      {
        systemState = SYSTEM_ERROR;  
        break;
      }
    
      case BUTTON_OFF_WAITING_FOR_RELAY:
      {
        systemState = BUTTON_OFF_RELAY_OFF;  
        break;
      }
    
      case INITIALIZING:
      {
        systemState = BUTTON_OFF_RELAY_OFF;  
        break;
      }
    }
  }

  Serial.print   ( "        New systemState = " ); Serial.println ( systemState ); 
  
  // Update the Blynk App widgets to the currentState
  updateBlynkWidgets();

  Serial.println ( "    exited checkRelay" );  

    
} // end checkRelay





// BLYNK_WRITE_DEFAULT GETS CALLED WHEN THERE IS NO SPECIFIC BLYNK_WRITE FOR THAT VIRTUAL PIN
// This makes it a flexible - and programmable - receiver
// The Config Portal can provide FLEXIBLE Virtual Pin to power this
BLYNK_WRITE_DEFAULT() 
{
  // THE VIRTUAL PIN THAT SENT THE MESSAGE TO BLYNK
  int writeVpin = request.pin; 
  
  Serial.print ( "\nBLYNK_WRITE_DEFAULT RECEIVED  SIGNAL FROM V");
  Serial.print ( writeVpin );
  Serial.println ( ": " );

  // Ugly but effective. take no action for the first second or so after Blynk connects
  if ( flushAtStartup ) 
  {
    Serial.print ( "     " ); Serial.println ( "* flushed *" );  
    return;  //////////////////////////////////////////////////////
  }
  
  // Print all parameter values for debugging
  for ( auto i = param.begin(); i < param.end(); ++i ) 
  {
    Serial.print ( "     " ); 
    Serial.print ( "* param = " ); Serial.println ( i.asString() );  
  }
  Serial.print ( "     " ); 
  Serial.println ( "*****\n" );  

  Serial.print ( "     " ); 
  Serial.print ( "Current systemState = " ); Serial.println ( systemState ); 


/*
systemState    BUTTON_OFF_RELAY_OFF, BUTTON_ON_WAITING_FOR_RELAY, BUTTON_ON_RELAY_ON, BUTTON_OFF_WAITING_FOR_RELAY, INITIALIZING
controlColor   BLYNK_GREEN,          BLYNK_RED,                   BLYNK_RED,          BLYNK_GREEN,                  BLYNK_GREEN,
controlWrite   LOW,                  HIGH,                        HIGH,               LOW,                          LOW,       
heartbeatColor BLYNK_GREEN,          BLYNK_GREEN,                 BLYNK_RED,          BLYNK_RED,                    BLYNK_GREY,
statusDisplay  " is OFF",            " turning ON",               " is ON",           " turning OFF",               " initializing",
*/
    
  // TAKE ACTION ON BUTTON PUSH
  if ( writeVpin == controlVpin ) // We have the ON/OFF input from Blynk
  {
    if ( ignoreQuickButtonPushes ) // Ignore button push and restore to previous state
    {
      Serial.println ( "\nPUSHBUTTON IGNORED\n" );  
      
      int controlButtonValue;
      if ( controlButtonON ) // restore to previous state
        controlButtonValue = 1;
      else // controlButton was OFF
        controlButtonValue = 0;
      
      // Set the controlButton back to its earlier state
      Blynk.virtualWrite ( controlVpin, controlButtonValue );        
    }
    else // Process good Control Button Push
    {
      // Get the ON-Off button value, turn the Sonoff ON or OFF
      int controlButtonValue = param[0].asInt();
      Serial.print ( "     " ); 
      Serial.print ( "Control Button Hit with value = " ); Serial.println ( controlButtonValue );  

      if ( controlButtonValue == 1 )
        controlButtonON = true;
      else
        controlButtonON = false;  



      if ( controlButtonON )
      switch ( systemState )
      {
        case BUTTON_OFF_RELAY_OFF:
        {
          systemState = BUTTON_ON_WAITING_FOR_RELAY;  
          turnSONOFFon();
          break;
        }
      
        case BUTTON_ON_WAITING_FOR_RELAY:
        {
          systemState = BUTTON_ON_WAITING_FOR_RELAY;  
          break;
        }
      
        case BUTTON_ON_RELAY_ON:
        {
          systemState = BUTTON_ON_RELAY_ON;  
          break;
        }
      
        case BUTTON_OFF_WAITING_FOR_RELAY:
        {
          systemState = BUTTON_ON_WAITING_FOR_RELAY;  
          turnSONOFFon();
          break;
        }
      
        case INITIALIZING: 
        {
          systemState = BUTTON_ON_WAITING_FOR_RELAY;  
          turnSONOFFon();
          break;
        }
      }
      else // button is OFF
      switch ( systemState )
      {
        case BUTTON_OFF_RELAY_OFF:
        {
          systemState = BUTTON_OFF_RELAY_OFF;  
          break;
        }
      
        case BUTTON_ON_WAITING_FOR_RELAY:
        {
          systemState = BUTTON_OFF_WAITING_FOR_RELAY;  
          turnSONOFFoff();
          break;
        }
      
        case BUTTON_ON_RELAY_ON:
        {
          systemState = BUTTON_OFF_WAITING_FOR_RELAY;  
          turnSONOFFoff();
          break;
        }
      
        case BUTTON_OFF_WAITING_FOR_RELAY:
        {
          systemState = BUTTON_OFF_WAITING_FOR_RELAY;  
          break;
        }
     
        case INITIALIZING:
        {
          systemState = BUTTON_OFF_WAITING_FOR_RELAY;  
          turnSONOFFoff();
          break;
        }
      } // end case stmt on OFF button push
      
      // We just got a good Control Button Push so...
      // Set a timer to IGNORE buttonpushes for a short while
      ignoreQuickButtonPushes = true;  
      ignoreQuickButtonPushesTimerID = myTimer.setTimeout ( ignoreQuickButtonPushesTimerDuration, ignoreQuickButtonPushesStop ); 
    
      // Update the Blynk App widgets to the currentState
      updateBlynkWidgets();
    
    } // end processing a good ON or OFF Control Button Push

  } // end of processing a Control Button Push

  else if ( writeVpin == resetVpin ) // We have an hit on the 'invisible' reset button
  {
    // Get the Reset button value, Hit Reset and reboot if = 1
    int buttonVal = param[0].asInt();
    Serial.print ( "     " ); 
    Serial.print ( "RESET Button Hit with value = " ); Serial.println ( buttonVal );  

    if ( buttonVal == 1 )
    {
      Serial.println ( " \n REBOOTING " );  
      Blynk.clearConfigData();
      delay ( 8000 );  
      ESP.restart();
    }
  }

  Serial.print ( "     " ); 
  Serial.print ( "New systemState = " ); Serial.println ( systemState ); 

  Serial.println ( "exited BLYNK_WRITE_DEFAULT" );  

} //end BLYNK_WRITE_DEFAULT





void ignoreQuickButtonPushesStop()
{
  ignoreQuickButtonPushes = false;
}





// TURN ON THE SONOFF SWITCH
//  MAKE THE BUTTON AND HEARTBEAT LED RED
//  MAKE THE DISPLAY SAY UNIT IS ON
void turnSONOFFon()
{
  Serial.println ( "\n***********************" );
  Serial.println (    "** TURNING SONOFF ON **" );  
  Serial.println (    "***********************\n" );
  
  digitalWrite ( SONOFF_RELAY_PIN, HIGH );  // Set the Sonoff Relay ON

  // VERIFY THE RELAY ACTUALLY SWITCHED A SECOND AFTER THE BUTTON PUSH
  verifyRelayTimerID = myTimer.setTimeout ( verifyRelayTimerDuration, checkRelay );  
  
} // end turnSONOFFon





// TURN OFF THE SONOFF SWITCH
//  MAKE THE BUTTON AND HEARTBEAT LED GREEN
//  MAKE THE DISPLAY SAY UNIT IS OFF
void turnSONOFFoff()
{
  Serial.println ( "\n************************" );
  Serial.println (    "** TURNING SONOFF OFF **" );  
  Serial.println (    "************************\n" );
  
  digitalWrite ( SONOFF_RELAY_PIN, LOW );  // Set teh Sonoff Relay OFF

  // VERIFY THE RELAY ACTUALLY SWITCHED A SECOND AFTER THE BUTTON PUSH
  verifyRelayTimerID = myTimer.setTimeout ( verifyRelayTimerDuration, checkRelay );  

} // end turnSONOFFoff





// Set up the STATIC (one time) colors and static labels (updateBlynkApp handles the dynamic data)
void initializeBlynkWidgets()
{ //return; 
  Serial.println ( "\ninitializeBlynkWidgets called: " );   
  Serial.print ( controlVpin );   Serial.print ( ", " );  
  Serial.print ( heartbeatVpin ); Serial.print ( ", " );  
  Serial.print ( displayVpin );   Serial.print ( ", " );  
  Serial.print ( resetVpin );     Serial.println();    

  // Update the Blynk App labels and (label) color
  Blynk.setProperty ( heartbeatVpin, "color",    BLYNK_GREY );  // make widgetlabels always white
  Blynk.setProperty ( displayVpin,   "color",    BLYNK_WHITE ); // make widgetlabels always white
  Blynk.setProperty ( resetVpin,     "color",    BLYNK_GREY );  // make widgetlabels always white  

  Blynk.setProperty ( controlVpin,   "onLabel",  "Push OFF" );  // When the Button is ON, it will say "Turn OFF"
  Blynk.setProperty ( controlVpin,   "offLabel", "Push ON"  );  // When the button is OFF, it will say "Turn ON"

  // Update the widget label with the value from the DYNAMIC_PARAMETER
  Blynk.setProperty ( controlVpin,   "label",    controlLabel ); 
  Blynk.setProperty ( heartbeatVpin, "label",    heartbeatLabel );  
  Blynk.setProperty ( displayVpin,   "label",    displayLabel ); 
  // Update the widget label with the value 
  Blynk.setProperty ( resetVpin,     "label",    "RESET" ); 
  
  Serial.println ( "initializeBlynkWidgets completed" );  
}





// Set up the operating colors and labels 
void updateBlynkWidgets()
{

  String controlButtonColor; 
  String heartbeatColor;
  int controlButtonWrite;  
  String statusDisplay = "";  
  
  Serial.print ( "            " );  
  Serial.print ( "updateBlynkWidgets called with systemState " ); 
  Serial.println ( systemState );  

/*
systemState    BUTTON_OFF_RELAY_OFF, BUTTON_ON_WAITING_FOR_RELAY, BUTTON_ON_RELAY_ON, BUTTON_OFF_WAITING_FOR_RELAY, INITIALIZING
controlColor   BLYNK_GREEN,          BLYNK_RED,                   BLYNK_RED,          BLYNK_GREEN,                  BLYNK_GREEN,
controlWrite   LOW,                  HIGH,                        HIGH,               LOW,                          LOW,       
heartbeatColor BLYNK_GREEN,          BLYNK_GREEN,                 BLYNK_RED,          BLYNK_RED,                    BLYNK_GREY,
statusDisplay  " is OFF",            " turning ON",               " is ON",           " turning OFF",               " initializing",
*/

  
  switch ( systemState )
  {
    case BUTTON_OFF_RELAY_OFF:
      controlButtonColor = BLYNK_GREEN;    
      heartbeatColor = BLYNK_GREEN;  
      controlButtonWrite = 0;  
      statusDisplay = " is OFF";  
      break;

    case BUTTON_ON_WAITING_FOR_RELAY:
      controlButtonColor = BLYNK_RED;    
      heartbeatColor = BLYNK_GREEN;  
      controlButtonWrite = 1;  
      statusDisplay = " turning ON";  
      break;

    case BUTTON_ON_RELAY_ON:
      controlButtonColor = BLYNK_RED;    
      heartbeatColor = BLYNK_RED;  
      controlButtonWrite = 1;  
      statusDisplay = " is ON";  
      break;

    case BUTTON_OFF_WAITING_FOR_RELAY:
      controlButtonColor = BLYNK_GREEN;    
      heartbeatColor = BLYNK_RED;  
      controlButtonWrite = 0;  
      statusDisplay = " turning OFF";  
      break;

    case INITIALIZING:
      controlButtonColor = BLYNK_GREEN;   
      heartbeatColor = BLYNK_GREY;  
      controlButtonWrite = 0;  
      statusDisplay = " Initializing";  
      break;

    case SYSTEM_ERROR: // REBOOTS on Error
      Serial.println ( "\nSYSTEM ERROR" ); 
      Serial.println (   "SYSTEM ERROR" ); 
      Serial.println (   "SYSTEM ERROR" ); 
      Serial.println (   "SYSTEM ERROR\n" ); 
      
      controlButtonColor = BLYNK_YELLOW;    
      heartbeatColor = BLYNK_YELLOW;  
      statusDisplay = " Error.  Restarting"; 
      break;

    default:
      Serial.println ( "\nDEFAULT CASE CALLED in updateBlynkWidgets" ); 
      controlButtonColor = BLYNK_YELLOW;    
      heartbeatColor = BLYNK_YELLOW;  
      statusDisplay = "I am so confused";  
      break;
  }

  Blynk.setProperty  ( controlVpin, "color", controlButtonColor );    
  Blynk.virtualWrite ( controlVpin, controlButtonWrite );  

  Blynk.setProperty ( heartbeatVpin, "color", heartbeatColor );  
  statusDisplay = String ( displayLabel ) + statusDisplay;  
  Blynk.virtualWrite ( displayVpin, statusDisplay );  


  ////////////////////////////////////////////////////////////////////////
  // RESTART SYSTEM ON systemState ERROR
  ////////////////////////////////////////////////////////////////////////
  if ( systemState == SYSTEM_ERROR )
  {
    Serial.println ( "Resetting in a few seconds...\n\n\n\n\n" );
    delay ( 3000 );  
    ESP.restart();
  }

  Serial.print ( "            " );  
  Serial.println( "exited updateBlynkWidgets" );  

} // end updateBlynkWidgets




// UPDATE DYNAMIC PARAMETERS (called by BLYNK_CONNECTED) 
//  1 - CONVERTS THE char INFO FROM THE CONFIG PORTAL OR COMPILER CONSTANTS TO THE int VALUES THEY NEED TO BE FOR USE IN A SKETCH
//  2 - UPDATES BLYNK WITH OTHER DYNAMIC PARAMETERS (WIDGET LABELS) 
void updateDynamicParameters()
{
  Serial.println ( "\nupdateDynamicParameters called with..." );  

  Serial.print ( "     " ); 
  Serial.print ( "controlVpinC/label = """ );     Serial.print ( controlVpinC );   Serial.print ( """/" ); Serial.print ( controlLabel ); 
  Serial.print ( ", heartbeatVpinC/label = """ ); Serial.print ( heartbeatVpinC ); Serial.print ( """/" ); Serial.print ( heartbeatLabel ); 
  Serial.print ( ", displayVpinC/label = """ );   Serial.print ( displayVpinC );   Serial.print ( """/" ); Serial.print ( displayLabel ); 
  Serial.println ();  

    
  // CONTROL VPIN - Convert char Virtual Pin numbers to int in preperation for Blynk connect
  controlVpin = atoi ( controlVpinC );
  if ( controlVpin < 0 || controlVpin > 255 )
  {
    controlVpin = atoi ( CONTROL_DEFAULT_VPIN );
    Serial.print ( "     " ); 
    Serial.print ( "** Bad controlVpin input value of " ); Serial.println ( controlVpinC );
  };
  Serial.print ( "     " ); 
  Serial.print ( "Set controlVpin/controlLabel to " ) ; Serial.print ( controlVpin ); Serial.print ( "/" ); Serial.println ( controlLabel );   


  // HEARTBEAT VPIN - Convert char Virtual Pin numbers to int in preperation for Blynk connect
  heartbeatVpin = atoi ( heartbeatVpinC );
  if ( heartbeatVpin < 0 || heartbeatVpin > 255 )
  {
    heartbeatVpin = atoi ( HEARTBEAT_LED_DEFAULT_VPIN );
    Serial.print ( "     " ); 
    Serial.print ( "** Bad heartbeatVpin input value of " ); Serial.println ( heartbeatVpinC );
  }
  Serial.print ( "     " ); 
  Serial.print ( "Set heartbeatVpin/heartbeatLabel to " ) ; Serial.print ( heartbeatVpin ); Serial.print ( "/" ); Serial.println ( heartbeatLabel );   

    
  // DISPLAY VPIN - Convert char Virtual Pin numbers to int in preperation for Blynk connect
  displayVpin = atoi ( displayVpinC );
  if ( displayVpin < 0 || displayVpin > 255 )
  {
    displayVpin = atoi ( DISPLAY_DEFAULT_VPIN );
    Serial.print ( "     " ); 
    Serial.print ( "** Bad displayVpin input value of " ); Serial.println ( displayVpinC );
  }
  Serial.print ( "     " ); 
  Serial.print ( "Set displayVpin/heartbeatLabel to " ) ; Serial.print ( displayVpin ); Serial.print ( "/" ); Serial.println ( displayLabel );   

    // RESET VPIN - Assign a value to resetVpion as the MAX of the previous 3 Vpins + 1
  resetVpin = max ( controlVpin, heartbeatVpin ); 
  resetVpin = max ( displayVpin, resetVpin ) + 1;  // max function only takes TWO parameters!
  Serial.print ( "     " ); 
  Serial.print ( "Set resetVpin to " ) ; Serial.println ( resetVpin );

  Serial.println ( "exited updateDynamicParameters\n" );  

} // end updateDynamicParameters





// LED HEARTBEAT
void heartbeatLEDblink()
/* Blink the on-board LED AND the Virtual Blynk LED
 * If LED is off, it turne the LEDs on, then sets a timer to turn LED off
 * When the timer triggers, same routine turns the LEDs off
 */
{
  if ( heartbeatLEDon == false ) 
  {
    // LED is OFF - turn it on, check the relay, set a timer to turn heartbeat back off
    Serial.print ( "\n... heartbeat of " ); Serial.print ( SKETCH_NAME ); 
    Serial.print ( " WiFi.status() = " ); Serial.print ( WiFi.status() );  
    
    digitalWrite ( LED_BUILTIN, LED_BUILTIN_HIGH );  // Turn On-board LED on
    Blynk.virtualWrite ( heartbeatVpin, BLYNK_LED_WIDGET_MAX_BRIGHTNESS ); // BLynk LED on

    checkRelay();  // check to see relay status and change systemState if needed
    // checkRelay calls updateBlynkWidgets

    // Set a timer to turn off the LEDs in a bit  
    heartbeatLEDdurationTimerID = myTimer.setTimeout ( heartbeatLEDduration, heartbeatLEDblink ); 
  }
  else // LED is ON. Turn it off
  {
    digitalWrite ( LED_BUILTIN, LED_BUILTIN_LOW ); // On-board LED off
    Blynk.virtualWrite ( heartbeatVpin, 000 );     // Blynk LED off
    
    Serial.println ( " ..." );
  }  
  
  heartbeatLEDon = ! heartbeatLEDon; // flip status
  
} //end heartbeatLEDblink





// SET UP BLYNK TIMER FOR HEARTBEAT (and anything else you may want later)
void setupBlynkTimers()
{
  Serial.println ( "Setting up Blynk timers" );  
  
  // Interval timer for heartbeatLED (Blynk LED and onboard LED
  // Introduce a little randomness to all the Heartbeat LEDs have a little different timing
  int randomizedHeartbeatLEDinterval = heartbeatLEDinterval + random ( -500, +500 );
  Serial.print ( "... HeartbeartLEDinterval = " ); Serial.println ( randomizedHeartbeatLEDinterval );
  heartbeatLEDtimerID = myTimer.setInterval ( randomizedHeartbeatLEDinterval, heartbeatLEDblink );  

  Serial.println ( "... Blynk timers set up." );  
  
} //end setupBlynkTimers





// THIS ROUTINE ALLOW THE SKETCH TO IGNORE ALL TEH GARBAGE BLYNK_WRITES WE OFTEN GET AT BLYNK_CONNECT
void endFlushAtStartup()
{
  flushAtStartup = false;
}





// CONNECT TO WLAN WITH OR WITHOUT WM
// Connect to Blynk once WiFi connection establishked
// Uses compiler switches for WiFiManager, SSL, other choices
void connectToWLANandBlynk()
{
  // Setup WLAN and Blynk
  Serial.print ( "\nSetting up WLAN and Blynk " );  
  #if USE_WM
    Serial.println ( "with WiFiManager" ); 
  #else
    Serial.println ( "WITHOUT WiFiManager" );  
  #endif
  
  #if USE_WM
    Serial.println ( "Starting Blynk.begin (with WM)" );  

    // SET UP THE CONFIG PORTAL CREDENTIALS AND CONNECTION INFO
    Blynk.setConfigPortalChannel ( 0 );  // 0 -> Use a random WiFi 2.4GHz channel for the Config Portal
    Blynk.setConfigPortal ( CONFIG_PORTAL_SSID, CONFIG_PORTAL_PASSWORD ); // Set up Config Portal SSID & Password
    Blynk.setConfigPortalIP ( IPAddress ( CONFIG_PORTAL_IPADDRESS ) ); // Set up IP address for COnfig Portal once connected to WiFi

    Serial.print ( "     " ); 
    Serial.print ( "Blynk.setConfigPortal(" ); 
    Serial.print ( CONFIG_PORTAL_SSID ); Serial.print ( "," );  
    Serial.print ( CONFIG_PORTAL_PASSWORD ); Serial.println ( ")" );   

    Serial.print ( "     " ); 
    Serial.print ( "Config Portal will be found at IP: " ); Serial.print ( IPAddress ( CONFIG_PORTAL_IPADDRESS ) );  
    Serial.println ( "\n" );  
    
    //Blynk.config ( blynkAuth );  // not needed with WM 
    Blynk.begin ( DEVICE_HOST_NAME ); // DHCP (router) device name
  
  #else//NOT using WM
    Serial.println ( "Starting WiFi.begin (no WM)" );  
    WiFi.begin ( WiFiSSID, WiFiPass );
    Serial.println ( "... waiting a few seconds for WiFi ..." );    
    #if ESP8266
      delay ( 7500 );  // For esp8266, it needs a delay to realize it has connected
    #endif
      
    // REBOOT if we do not have a good WiFi connection
    if ( WiFi.status() != WL_CONNECTED )
    {
      Serial.println ( "Resetting in a few seconds...\n\n\n\n\n" );
      delay ( 3000 );  
      ESP.restart();
    } 
    
    // Configure and launch Blynk
    Blynk.config ( blynkAuth );
    Blynk.connect ( 2500 ); // Don't get stuck hanging for more than 2500 millis.
  #endif // using WM
  
  if ( Blynk.connected() ) 
  {
    #if ( USE_LITTLEFS || USE_SPIFFS)
      Serial.println ( "\nBlynk using " + String ( CurrentFileFS ) + " connected. Board Name : " + Blynk.getBoardName() );
    #else
      Serial.println ( "\nBlynk using EEPROM connected. Board Name : " + Blynk.getBoardName() );
      Serial.printf ( "EEPROM size = %d bytes, EEPROM start address = %d / 0x%X\n", EEPROM_SIZE, EEPROM_START, EEPROM_START );
    #endif  
        
    Serial.println ( "Blynk connected just fine" ); 
    Serial.print   ( "  IP address  " ); Serial.println ( WiFi.localIP() ) ;
    Serial.print   ( "  MAC address " ); Serial.println ( WiFi.macAddress() );  
    Serial.println();  
  }
  else Serial.println ( "Blynk NOT CONNECTED \n\n" );  
} // end connectToWLANandBlynk





// BLYNK_CONNECTED GETS CALLED WHEN CONNECTING TO BLYNK SERVERS
// GETS CALLED IMMEDIATELY ON FIRST CONNECT TO BLYNK SERVER, TOO
BLYNK_CONNECTED()
{
  Serial.println ( "\nBLYNK_CONNECTED..." );  

  // This timer corrects a Blynk bug, allows all initial BLYNK_WRITEs to get flushed upon connect.
  //It's a ONE-TIME timer
  flushAtStartupTimerID = myTimer.setTimeout ( flushAtStartupTimerDuration, endFlushAtStartup );  

  // Convert the Config Portal (or compiler constant) Virtual Pin char values to Blynk-usable int
  updateDynamicParameters();  

  // Set up the Blynk App with all the static info (colors and labels)
  initializeBlynkWidgets();

  // Set up the working Blynk App color and label info
  updateBlynkWidgets();
  
} // end BLYNK_CONNECTED





// BLYNK_APP_CONNECTED GETS CALLED WHEN APP CONNECTS TO BLYNK SERVERS
// IT IS NOT SUPER RELIABLE !  
BLYNK_APP_CONNECTED()
{
  Serial.println ( "\nBLYNK_APP_CONNECTED..." );  

  // Set up the Blynk App with all teh static info (colors and labels)
  initializeBlynkWidgets();

  // Set up the working Blynk App color and label info
  updateBlynkWidgets();

    
} // end BLYNK_APP_CONNECTED
