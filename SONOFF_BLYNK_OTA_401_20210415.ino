#define SKETCH_NAME "SONOFF BLYNK OTA"
#define SERIAL_SPEED 230400

#ifndef ESP8266
  #error Hey this is for Sonoff and ESP8266 ONLY!
#endif

/* Version 401 
 * - added 4th SONOFF - Boris
 * - used EEPROM to remember the displayLabel
 *   I tried LittleFS, but it too up too much room
 *   In the process, I bracketed SERIAL_PRINT statementsn OTA and TELNET to reduce code size
 *   Nothing got the code small eough for SONOFF. (Fine for regular ESP 8266)
 *   Moving to EEPROM for displayLabel make a lot of room available. 
 *   
 * - OTA implemented along with TelNet using Andreas Spiess' code from YouTube. Excellent. 
 */

/*  
 * The Arduino/Blynk sketch SONOFF 301 OTA.ino is a fully-developed SONOFF control system using 
 * the powerful BlynkSimpleESP8266_SSL_WM WiFiManager (WM) libraries  for ESP8266. 
 * See https://github.com/thorathome/Blynk_Examples
 * See also https://github.com/khoih-prog/Blynk_WM for the Blynk WiFi Manager library
 * 
 * It demonstrates
 * * WiFiManager Config Portal configuration and use
 * * Use of compiler constants for compile-time selection
 * * Use of Blynk's BLYNK_WRITE_DEFAULT() flexible capability
 *
 * This sketch creates a simple Blynk on/off switch, an indicator Blynk LED and a display panel 
 * to tell me what this is controlling.
 * It lets me choose at runtime (without recompile or download to SONOFF - kind of a pain) the
 * * SONOFF device name
 * * Blynk Virtual Pins I will use for this particular device
 *
  * It requires:
 * * Blynk Button in SWITCH mode
 * * Blynk LED
 * * Blynk Value Display
 * * (optional) Blynk Button to force the WiFi Manager Config Portal to reset
 *              The DRD (Double Reset Detector) can do the same thing.
 * * Blynk master library installed
 * * Blynk_WiFiManager installed
 * * SONOFF Basic (ESP8266) device and the ability to flash it. (See elsewhere.)  
 *
 * As in all Blynk sketches using WiFi, you will need
 * * Your WiFi SSID or SSIDs and passwords
 * * Your Blynk authcode or authcodes
 * * Your Blynk server URL (Main US Blynk server is blynk-cloud.com.)
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
 * using #include files (but could be made selectable in the Config Portal, itself.)
 *
 */



#define USE_OTA true
//define USE_OTA false

#if USE_OTA
  #define USE_TELNET true
  //#define USE_TELNET false
#endif

/* this controls library loads, the recurring call in <loop> and TelNetStream output */
#if USE_OTA && USE_TELNET
  #include "OTABlynkTelNet.h" // stub from Andreas Spiess in my own directory
#elif USE_OTA && ! USE_TELNET
  #include "OTABlynk.h"       // stub from Andreas Spiess in my own directory
#endif



/// Need to use EEPROM due to size constraints of LittleFS (too large) in SONOFF (along with SSL)
/// Must have this BEFORE the #includes
#define USE_LITTLEFS      false   // true = LittleFS  false = EEPROM
#define USE_SPIFFS        false   // Never ever use SPIFFS 

// UNUSED We use a EEPROM file to maintain the device label seen on the widgets
#define DISPLAY_LABEL_FILE_NAME  "/SONOFF_DISPLAY.txt"



#define SERIAL_PRINT true         // Space saver (not needed, it turns out)
//#define SERIAL_PRINT false



#define BLYNK_PRINT       Serial  // Generates Blynk debug prints. Comment out if not needed, saving space
#define BLYNK_WM_DEBUG    0       // 0 - 3



#include "MY_WIFI_CREDENTIALS.h"   // #defines MY_WIFI_SSID/2 AND MY_WIFI_PASSWORD/2
#include "MY_BLYNK_CREDENTIALS.h"  // #defines MY_BLYNK_SERVER and MY_xxx_AUTHCODE
#include "ESP_LED_BUILTINS.h"      // #defines LED_BUILTIN_HIGH and _LOW for uniform use across devices
#include "MY_BLYNK_COLORS.h"       // #defines a bunch of handy Blynk colors



// ONLY ONE OF THESE CAN BE true
// I use several different devices, have named them from my childhood
#define LARRY       false   // <===== updated 31 Mar 2021
#define CURLY       false   // updated
#define MOE         false   // updated
#define BORIS       true   // updated
#define SONOFF_DEV  false

// Look for more than one, or none selected
#if ( LARRY && ( CURLY || MOE   || BORIS || SONOFF_DEV ) ) // only Larry
 || ( CURLY && ( LARRY || MOE   || BORIS || SONOFF_DEV ) ) // only Curly
 || ( MOE   && ( LARRY || CURLY || BORIS || SONOFF_DEV ) ) // only Moe
 || ( BORIS && ( LARRY || CURLY || MOE   || SONOFF_DEV ) ) // only Boris
 || ( SONOFF_DEV && ( LARRY || CURLY || MOE || BORIS ) )   // only SONOFF_DEV
 || ! ( LARRY || CURLY || MOE || BORIS || SONOFF_DEV )      // at least one is selected

 #error "ERROR Selecting Target SONOFF Platform" 
#endif


// ALL 3 Devices use 1 Blynk Authcode!  (Development is different.)
// Larry uses Virtual Pins 20-23, Curly is 30-33, Moe is 40-43, Boris is 50-53
// Development uses 10-13
#if LARRY
  #define DEVICE_NAME            "SONOFF-LARRY"  // Choose device
  #define CONTROL_VPIN   20
  #define CONTROL_LABEL          "LARRY (INT.)"   // Shows up above ON/OFF button
  #define HEARTBEAT_VPIN 21
  #define HEARTBEAT_LABEL        "Working"
  #define DISPLAY_VPIN   22
  #define UPDATE_VPIN    23
  #define CONFIG_PORTAL_SSID     "CONFIG_LARRY"  // IP and passwords set beloe
  #define UPDATE_LABEL           "UPDATE LARRY"
  #define MY_BLYNK_AUTHCODE      MY_REAL_SONOFF_AUTHCODE  
#elif CURLY
  #define DEVICE_NAME            "SONOFF-CURLY"  // Choose device
  #define CONTROL_VPIN   30
  #define CONTROL_LABEL          "CURLY (EXT.)"   // Shows up above ON/OFF button
  #define HEARTBEAT_VPIN 31
  #define HEARTBEAT_LABEL        "Working"
  #define DISPLAY_VPIN   32
  #define UPDATE_VPIN    33
  #define UPDATE_LABEL           "UPDATE CURLY"
  #define CONFIG_PORTAL_SSID     "CONFIG_CURLY"  // IP and passwords set beloe
  #define MY_BLYNK_AUTHCODE      MY_REAL_SONOFF_AUTHCODE  
#elif MOE
  #define DEVICE_NAME            "SONOFF-MOE"    // Choose device
  #define CONTROL_VPIN   40
  #define CONTROL_LABEL          "MOE (EXT.)"   // Shows up above ON/OFF button
  #define HEARTBEAT_VPIN 41
  #define HEARTBEAT_LABEL        "Working"
  #define DISPLAY_VPIN   42
  #define UPDATE_VPIN    43
  #define UPDATE_LABEL           "UPDATE MOE"
  #define CONFIG_PORTAL_SSID     "CONFIG_MOE"    // IP and passwords set beloe
  #define MY_BLYNK_AUTHCODE      MY_REAL_SONOFF_AUTHCODE  
#elif BORIS
  #define DEVICE_NAME            "SONOFF-BORIS"  // Choose device
  #define CONTROL_VPIN   50
  #define CONTROL_LABEL          "BORIS (INT.)"   // Shows up above ON/OFF button
  #define HEARTBEAT_VPIN 51
  #define HEARTBEAT_LABEL        "Working"
  #define DISPLAY_VPIN   52
  #define UPDATE_VPIN    53
  #define UPDATE_LABEL           "UPDATE BORIS"
  #define CONFIG_PORTAL_SSID     "CONFIG_BORIS"  // IP and passwords set beloe
  #define MY_BLYNK_AUTHCODE      MY_REAL_SONOFF_AUTHCODE  
  
#elif SONOFF_DEV
  #define DEVICE_NAME            "SONOFF_DEV"   // Choose SONOFF_DEV development
  #define CONTROL_VPIN   10
  #define CONTROL_LABEL          "SONOFF (dev)" // Shows up above ON/OFF button
  #define HEARTBEAT_VPIN 11
  #define HEARTBEAT_LABEL        "Working"
  #define DISPLAY_VPIN   12
  #define UPDATE_VPIN    13
  #define UPDATE_LABEL           "UPDATE SONOFF_DEV"
  #define CONFIG_PORTAL_SSID     "CONFIG_SONOFF_DEV" // IP and passwords set beloe
  #define MY_BLYNK_AUTHCODE      MY_WORKING_DEVELOPMENT_AUTHCODE  
#endif

#if SONOFF_DEV
  #define SONOFF_RELAY_PIN  5    // ESP8266 RELAY ON/OFF
#else
  #define SONOFF_RELAY_PIN  12   // SONOFF RELAY ON/OFF
  #define LED_BUILTIN 13         // SONOFF ONBOARD LED
#endif



#define BLYNK_HOST_NAME DEVICE_NAME             // Shows up in Config Portal as Tab Name
                                                // ALSO shows up in Router table as device name

#define OTA_BOARD_NAME  DEVICE_NAME             // used in OTA only


// Cutting down on the brightness of the Blynk LED Widget
#define BLYNK_LED_WIDGET_MAX_BRIGHTNESS 150     // Led Widget is [0-255]  150 = not very bright (from 255)



#define USE_SSL false
#if USE_SSL
  #include "MY_BLYNK_WIFIMANAGER_SETUP.h"   // SSL for Blynk not working on ESP8266 devices as of March 2021
#else
  #include "MY_BLYNK_WIFIMANAGER_SETUP_NO_SSL.h" 
#endif




// The Blynk Widgets display the device name AND a changable DisplayLabel.
// See UPDATE_VPIN and UPDATE_LABEL above
#define DISPLAY_LABEL_LENGTH 16 
char    displayLabel[DISPLAY_LABEL_LENGTH+1]     = DEVICE_NAME; 




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
// This flag prevents State Machine changes from recursive calls of the State Machines
bool protectSystemState = false; 

// State Machine 6 States
#define BUTTON_OFF_RELAY_OFF          1 // Start with Button Off and Relay Off
#define BUTTON_ON_WAITING_FOR_RELAY   2 // Button goes ON, Relay still Off
#define BUTTON_ON_RELAY_ON            3 // Button still On, Relay went On
#define BUTTON_OFF_WAITING_FOR_RELAY  4 // Button goes Off, Relay still On
#define INITIALIZING                  5 // Just getting started
#define SYSTEM_ERROR                  6 // ALL SCREWED UP
int systemState = INITIALIZING; 

// Basic States of the ON/Off Button and the SONOFF Relay
bool controlButtonON = false;   // The initial state of the Widget Button
bool relayON =         false;   // The initial state of the Relay
bool switchingRelay =  false;   // Is the relay being switched right now? 





// SETUP WIFI, BLYNK, HEARTBEAT
void setup() 
{
  // HELP DEBUG THIS SKETCH WITH GOOD PRINTOUTS
#if SERIAL_PRINT
  Serial.begin ( SERIAL_SPEED );
  delay ( 500 );  
  Serial.println ( "\n\n=======================================" );
  Serial.print ( SKETCH_NAME ); 
  Serial.println ( " ** Using SSL **" );  
  #if USE_LITTLEFS
    listDir ( "/" );
  #endif
#endif

  // Initialize Onboard LED - turn ON during startup
  pinMode ( LED_BUILTIN, OUTPUT );  
  digitalWrite ( LED_BUILTIN, LED_BUILTIN_HIGH ); 

  // Initialize SONOFF relay pin, Turn it off
  pinMode ( SONOFF_RELAY_PIN, OUTPUT );  
  digitalWrite ( SONOFF_RELAY_PIN, LOW ); // turn that sucker OFF asap 

  // There's a Blynk bug that causes all sorts of erroreous BLYNK_WRITEs immediately upon connect
  // This kills all new BLYNK_WRITEs for a second
  flushAtStartup = true;  // Flush everthing BLYNK_WRITE_DEFAULT sees for a second or so at startup

  
  systemState = INITIALIZING;  
  protectSystemState = false;  
  
  connectToWLANandBlynk();  // Connect to WiFi, then to Blynk server

#if USE_OTA
  // Set up OTA
  setupOTA ( OTA_BOARD_NAME ); // already connected to WiFi

  // Blink the onboard LED 5 times to signal goodness
  for ( int i = 1; i <= 5; i++)
  {
    digitalWrite ( LED_BUILTIN, LED_BUILTIN_HIGH );   delay ( 100 ); 
    digitalWrite ( LED_BUILTIN, LED_BUILTIN_LOW );    delay ( 100 );        
  }
#endif
  
  // Read Label widgets deviceLabel from LittleFS or EEPROM
  getLabelString();  
  
  setupBlynkTimers();  // Establish Blynk timers after Blynk is connected

  // Make sure the relay is off (turnSONOFFoff uses a Blynk timer)
  turnSONOFFoff();  // this uses a timer, so timers must be set up above in setupBlynkTimers
  relayON = true; controlButtonON = false;  // trick checkRelay into executing fully and set the proper state
  
  // Set Blynk Virtual Heartbeat and LED_BUILTIN OFF
  heartbeatLEDon = false;  
  digitalWrite ( LED_BUILTIN, LED_BUILTIN_LOW ); 
  Blynk.virtualWrite ( HEARTBEAT_VPIN, 000 );  
  
  heartbeatLEDblink(); // start first heartbeat (uses timers, too)
  
#if SERIAL_PRINT
  #if USE_LITTLEFS
    listDir ( "/" );
  #endif    
  Serial.println ( "\nSetup complete \n*******************************\n" );    
#endif
} //end setup





// KEEPING IT SIMPLE
void loop()
{
  Blynk.run();
  myTimer.run();  

  #if USE_OTA  // Do not use this for ESP32 - find RTOS code
    ArduinoOTA.handle();
  #endif
} // end loop





// BLYNK_WRITE_DEFAULT GETS CALLED WHEN THERE IS NO SPECIFIC BLYNK_WRITE FOR THAT VIRTUAL PIN
// This makes it a flexible - and programmable - receiver
// The Config Portal can provide FLEXIBLE Virtual Pin to power this
BLYNK_WRITE_DEFAULT() 
{
  // THE VIRTUAL PIN THAT SENT THE MESSAGE TO BLYNK
  int writeVpin = request.pin; 

#if SERIAL_PRINT
  Serial.print ( "\nBLYNK_WRITE_DEFAULT received a signal from V");
  Serial.print ( writeVpin );
  Serial.println ( ": " );
#endif
  
  // Ugly but effective. take no action for the first second or so after Blynk connects
  if ( flushAtStartup ) 
  {
#if SERIAL_PRINT
    Serial.print ( "     " ); Serial.println ( "* flushed *" );  
#endif
    return;  //////////////////////////////////////////////////////
  }
  
#if SERIAL_PRINT 
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
  if ( protectSystemState ) Serial.println ( "protectSystemState = TRUE" ); 
#endif
   
  // TAKE ACTION ON BUTTON PUSH
  if ( writeVpin == CONTROL_VPIN ) // We have the ON/OFF input from Blynk App
  {
    if ( ignoreQuickButtonPushes ) // Ignore button push and restore to previous state
    {
#if SERIAL_PRINT 
      Serial.println ( "\n  >>PUSHBUTTON IGNORED<<\n" );  
#endif
      int controlButtonValue;
      if ( controlButtonON ) // restore to previous state
        controlButtonValue = 1;
      else // controlButton was OFF
        controlButtonValue = 0;
    }
    else // Process good Control Button Push
    {
      // Get the ON-Off button value, turn the Sonoff ON or OFF
      int controlButtonValue = param[0].asInt();
#if SERIAL_PRINT 
      Serial.print ( "     " ); 
      Serial.print ( "Control Button Hit with value = " ); Serial.println ( controlButtonValue );  

      if ( systemState < 1 || systemState > 5 ) 
      { 
        Serial.println ( "SYSTEMSTATE ERROR in BLYNK_WRITE_DEFAULT(Control Button Push) " ); Serial.println ( systemState); 
      }

#endif

      if ( controlButtonValue == 1 )
        controlButtonON = true;
      else
        controlButtonON = false;  

      // We have a good Control Button Push, so we will now change state
      // Protect theis State Machine while it's running from timer-initiated routines also working the State machine
      if ( ! protectSystemState )
      {
      
        // We just got a good Control Button Push, and now will change state in teh State Machine
        // Prevent other instances of ther State Machine from changing state now
        protectSystemState = true;  
        
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
        } // end Control Button ON switch/case stmt
        
        else // Control Button is OFF
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

        } // end case stmt on good button push
        
        // We are done working the FSM now, so can open it up for other instances
        protectSystemState = false; 
          
      } // end protection for FSM changes on GOOD button push
          
      // We just got a good Control Button Push so...
      // Set a timer to IGNORE buttonpushes for a short while
      ignoreQuickButtonPushes = true;  
      ignoreQuickButtonPushesTimerID = myTimer.setTimeout ( ignoreQuickButtonPushesTimerDuration, ignoreQuickButtonPushesStop ); 
    
      // Update the Blynk App widgets to the currentState
      updateBlynkWidgets();
    
    } // end processing a good ON or OFF Control Button Push

  } // end of processing any (good or bad) Control Button Push

  else if ( writeVpin == UPDATE_VPIN ) // We have an hit on the 'invisible' update labels button
  {
    // Get the Update Label value, Hit Reset and reboot if = "RESET"
    String buttonVal = param[0].asStr();

#if SERIAL_PRINT 
    Serial.print ( "     " ); 
    Serial.print ( "UPDATE LABEL INPUT with value  = " ); Serial.println ( buttonVal );  
#endif
    if ( buttonVal == "RESET" || buttonVal == "Reset" || buttonVal == "Reboot" ) 
    {
#if SERIAL_PRINT 
      Serial.println ( " \n REBOOTING " );  
#endif
      delay ( 5000 );  
      ESP.restart();      
    }
    else
    {
      // We have a new displayLabel value to set
      buttonVal.toCharArray ( displayLabel, DISPLAY_LABEL_LENGTH + 1  );  
#if SERIAL_PRINT 
      Serial.print ( "UPDATE LABEL OUTPUT with value = " ); Serial.println ( displayLabel );  
#endif

      // Save updated Label in LittleFS or EEPROM for next time
      saveLabelString();  
      
      // Now update the Blynk Widgets
      Blynk.setProperty ( DISPLAY_VPIN,   "label",    displayLabel );    
    }
  }

#if SERIAL_PRINT 
  Serial.print ( "     " ); 
  Serial.print ( "New systemState = " ); Serial.println ( systemState ); 

  Serial.println ( "exited BLYNK_WRITE_DEFAULT" );  
#endif
} //end BLYNK_WRITE_DEFAULT





// CHECK TO SEE IF THE RELAY STATE MATCHES THE CONTROL
// Runs on timers aand from heartbeat
void checkRelay()
{
  Serial.print ( "\n    checkRelay called: " ); 

  bool newRelayON;    
#if ! SONOFF_DEV
  if ( digitalRead ( SONOFF_RELAY_PIN ) == 1 )
    newRelayON = true;
  else
    newRelayON = false;  
#else
  newRelayON = controlButtonON;  // Match the button while developing without an actual SONOFF
#endif

#if SERIAL_PRINT 
  Serial.print ( ", relayON = " ); Serial.print ( relayON );  
  Serial.print ( ", controlButtonON = " ); Serial.println ( controlButtonON ); 

  Serial.print ( "        Current systemState = " ); Serial.println ( systemState ); 

  if ( systemState < 1 || systemState > 5 ) 
  { Serial.println ( "SYSTEMSTATE ERROR in updateBlynkWidgets " ); Serial.println ( systemState); }
#endif
  
  if ( newRelayON == relayON )
  { 
  #if SERIAL_PRINT 
    Serial.print   ( "        No change in relay status. systemState = " ); Serial.println ( systemState ); 
  #endif
  }
  else  // relay has changed state - now change systemState if needed
  {
    relayON = newRelayON;  

    // Relay state has changed, so we will change systemState
    // Prevent this from overwriting another change to the systemState elsewhere
    if ( ! protectSystemState )
    {
      // Don't let anyone else mess with teh FSM during this process
      protectSystemState = true;  
      if ( relayON )
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
          turnSONOFFoff();  
          break;
        }
      } // end relay = ON switch/case
      
      else // relay is OFF
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
      } // end relay is OFF switch/case statement

      // We ran in a protectedSystemState = true mode while FSM was running
      protectSystemState = false;  
      
    } // end if protectSystemsState
  
#if SERIAL_PRINT 
    Serial.print   ( "        New systemState = " ); Serial.println ( systemState ); 
#endif
  }
  
  // Update the Blynk App widgets to the currentState every time checkRelay runs
  updateBlynkWidgets();

#if SERIAL_PRINT 
  Serial.println ( "    exited checkRelay" );  
#endif
} // end checkRelay





void ignoreQuickButtonPushesStop()
{
  ignoreQuickButtonPushes = false;
}





// TURN ON THE SONOFF SWITCH
//  MAKE THE BUTTON AND HEARTBEAT LED RED
//  MAKE THE DISPLAY SAY UNIT IS ON
void turnSONOFFon()
{
#if SERIAL_PRINT 
  Serial.println ( "\n***********************" );
  Serial.println (    "** TURNING SONOFF ON **" );  
  Serial.println (    "***********************\n" );
#endif
#if USE_TELNET
  TelnetStream.println ( "\n************************" );
  TelnetStream.println (    "** TURNING SONOFF ON **" );  
  TelnetStream.println (    "***********************\n" );
#endif
  
  digitalWrite ( SONOFF_RELAY_PIN, HIGH );  // Set the Sonoff Relay ON

  // VERIFY THE RELAY ACTUALLY SWITCHED A SECOND AFTER THE BUTTON PUSH
  verifyRelayTimerID = myTimer.setTimeout ( verifyRelayTimerDuration, checkRelay );  
  
} // end turnSONOFFon





// TURN OFF THE SONOFF SWITCH
//  MAKE THE BUTTON AND HEARTBEAT LED GREEN
//  MAKE THE DISPLAY SAY UNIT IS OFF
void turnSONOFFoff()
{
#if SERIAL_PRINT 
  Serial.println ( "\n************************" );
  Serial.println (    "** TURNING SONOFF OFF **" );  
  Serial.println (    "************************\n" );
#endif
#if USE_TELNET
  TelnetStream.println ( "\n************************" );
  TelnetStream.println (    "** TURNING SONOFF OFF **" );  
  TelnetStream.println (    "************************\n" );
#endif
  
  digitalWrite ( SONOFF_RELAY_PIN, LOW );  // Set teh Sonoff Relay OFF

  // VERIFY THE RELAY ACTUALLY SWITCHED A SECOND AFTER THE BUTTON PUSH
  verifyRelayTimerID = myTimer.setTimeout ( verifyRelayTimerDuration, checkRelay );  

} // end turnSONOFFoff





// Set up the STATIC (one time) colors and static labels (updateBlynkApp handles the dynamic data)
void initializeBlynkWidgets()
{   
#if SERIAL_PRINT 
  Serial.println ( "\ninitializeBlynkWidgets called: " );   
  Serial.print ( CONTROL_VPIN );   Serial.print ( ", " );  
  Serial.print ( HEARTBEAT_VPIN ); Serial.print ( ", " );  
  Serial.print ( DISPLAY_VPIN );   Serial.print ( ", " );  
  Serial.print ( UPDATE_VPIN );    Serial.println();    
#endif
  
  // Update the Blynk App labels and (label) color
  Blynk.setProperty ( HEARTBEAT_VPIN, "color",    BLYNK_GREY );  // make widgetlabels always white
  Blynk.setProperty ( DISPLAY_VPIN,   "color",    BLYNK_WHITE ); // make widgetlabels always white
  Blynk.setProperty ( UPDATE_VPIN,    "color",    BLYNK_GREY );  // make widgetlabels always white  

  Blynk.setProperty ( CONTROL_VPIN,   "onLabel",  "ON"   );  // When the Button is ON, it will say "OFF"
  Blynk.setProperty ( CONTROL_VPIN,   "offLabel", "OFF"  );  // When the button is OFF, it will say "ON"

  // Update the widget label with the value from the DYNAMIC_PARAMETER
  Blynk.setProperty ( CONTROL_VPIN,   "label",    CONTROL_LABEL ); 
  Blynk.setProperty ( HEARTBEAT_VPIN, "label",    HEARTBEAT_LABEL );  
  Blynk.setProperty ( DISPLAY_VPIN,   "label",    displayLabel ); 
  Blynk.setProperty ( UPDATE_VPIN,    "label",    UPDATE_LABEL ); 

#if SERIAL_PRINT 
  Serial.println ( "initializeBlynkWidgets completed" );  
#endif
}




// Set up the operating colors and labels 
void updateBlynkWidgets()
{

  String controlButtonColor = BLYNK_WHITE;; 
  String heartbeatColor = BLYNK_WHITE;
  int controlButtonWrite = -1;  
  String statusDisplay = "";  

#if SERIAL_PRINT 
  Serial.print ( "            " );  
  Serial.print ( "updateBlynkWidgets called with systemState " ); 
  Serial.println ( systemState );  
  if ( systemState < 1 || systemState > 5 ) 
  { 
    Serial.println ( "SYSTEMSTATE ERROR in updateBlynkWidgets " ); Serial.println ( systemState); 
    return;
  }
#endif
  
/*
systemState    BUTTON_OFF_RELAY_OFF, BUTTON_ON_WAITING_FOR_RELAY, BUTTON_ON_RELAY_ON, BUTTON_OFF_WAITING_FOR_RELAY, INITIALIZING
controlColor   BLYNK_GREEN,          BLYNK_RED,                   BLYNK_RED,          BLYNK_GREEN,                  BLYNK_GREEN,
controlWrite   LOW,                  HIGH,                        HIGH,               LOW,                          LOW,       
heartbeatColor BLYNK_GREEN,          BLYNK_GREEN,                 BLYNK_RED,          BLYNK_RED,                    BLYNK_GREY,
statusDisplay  " is OFF",            " turning ON",               " is ON",           " turning OFF",               " initializing",
*/

  // Do not let Button-triggered or timer-triggered changes to systemState screw up this routine
  if ( protectSystemState )
  {
#if SERIAL_PRINT 
    Serial.println ( "  >> updateBlynkWidgets called while protectedSystemState = true" );    
#endif
  }
  
  else // OK to execute on the BlynkAPp updates
  { 
    protectSystemState = true;  

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
#if SERIAL_PRINT 
        Serial.println ( "\nSYSTEM ERROR" ); 
        Serial.println (   "SYSTEM ERROR" ); 
        Serial.println (   "SYSTEM ERROR" ); 
        Serial.println (   "SYSTEM ERROR\n" ); 
#endif
        controlButtonColor = BLYNK_YELLOW;    
        heartbeatColor = BLYNK_YELLOW;  
        controlButtonWrite = 0;  
        statusDisplay = " Error.  Restarting"; 
        break;

      default:
#if SERIAL_PRINT 
        Serial.println ( "\nDEFAULT CASE CALLED in updateBlynkWidgets" ); 
#endif
        controlButtonColor = BLYNK_YELLOW;    
        heartbeatColor = BLYNK_YELLOW;  
        controlButtonWrite = 0;  
        statusDisplay = "I am so confused";  
        break;
    }

    Blynk.setProperty  ( CONTROL_VPIN, "color", controlButtonColor );    
    Blynk.virtualWrite ( CONTROL_VPIN, controlButtonWrite );  

    Blynk.setProperty ( HEARTBEAT_VPIN, "color", heartbeatColor );  
    statusDisplay = String ( displayLabel ) + statusDisplay;  
    Blynk.virtualWrite ( DISPLAY_VPIN, statusDisplay );  


    ////////////////////////////////////////////////////////////////////////
    // RESTART SYSTEM ON systemState ERROR
    ////////////////////////////////////////////////////////////////////////
    if ( systemState == SYSTEM_ERROR )
    {
#if SERIAL_PRINT 
      Serial.println ( "SYSTEM ERROR from updateBlynkWidgets.\nResetting in a few seconds...\n\n\n\n\n" );
#endif
      ESP.restart();
    }

    protectSystemState = false;  
  } // end of protectedSystemState = false

#if SERIAL_PRINT 
  Serial.print ( "            " );  
  Serial.println( "exited updateBlynkWidgets" );  
#endif
} // end updateBlynkWidgets





// LED HEARTBEAT
void heartbeatLEDblink()
/* Blink the on-board LED AND the Virtual Blynk LED
 * If LED is off, it turne the LEDs on, then sets a timer to turn LED off
 * When the timer triggers, same routine turns the LEDs off
 */
{
  if ( heartbeatLEDon ) 
  {
    digitalWrite ( LED_BUILTIN, LED_BUILTIN_LOW ); // On-board LED off
    Blynk.virtualWrite ( HEARTBEAT_VPIN, 000 );     // Blynk LED off

#if SERIAL_PRINT 
    Serial.println ( " ..." );
#endif
#if USE_TELNET
    TelnetStream.println ( " ..." );
#endif
  }  
  else // LED is OFF. Turn it ON
  {
    // LED is OFF - turn it on, check the relay, set a timer to turn heartbeat back off
#if SERIAL_PRINT 
    Serial.print ( "\n... heartbeat of " ); Serial.print ( DEVICE_NAME ); 
    Serial.print ( " WiFi.status() = " ); Serial.print ( WiFi.status() );  
#endif
#if USE_TELNET
    TelnetStream.print ( "\n... heartbeat of " ); TelnetStream.print ( DEVICE_NAME ); 
    TelnetStream.print ( " WiFi.status() = " ); TelnetStream.print ( WiFi.status() );  
#endif
    
    analogWrite ( LED_BUILTIN, 750 );                                       // Turn On-board LED on
    Blynk.virtualWrite ( HEARTBEAT_VPIN, BLYNK_LED_WIDGET_MAX_BRIGHTNESS ); // Blynk LED on

    checkRelay();  // check to see relay status and change systemState if needed
    // checkRelay calls updateBlynkWidgets

    // Set a timer to turn off the LEDs in a bit  
    heartbeatLEDdurationTimerID = myTimer.setTimeout ( heartbeatLEDduration, heartbeatLEDblink ); 
  }
  
  heartbeatLEDon = ! heartbeatLEDon; // flip status
  
} //end heartbeatLEDblink





// SET UP BLYNK TIMER FOR HEARTBEAT (and anything else you may want later)
void setupBlynkTimers()
{
  // Interval timer for heartbeatLED (Blynk LED and onboard LED
  // Introduce a little randomness to all the Heartbeat LEDs have a little different timing
  int randomizedHeartbeatLEDinterval = heartbeatLEDinterval + random ( -500, +500 );

#if SERIAL_PRINT 
  Serial.println ( "Setting up Blynk timers" );  
  
  Serial.print ( "... HeartbeartLEDinterval = " ); Serial.println ( randomizedHeartbeatLEDinterval );
#endif

  heartbeatLEDtimerID = myTimer.setInterval ( randomizedHeartbeatLEDinterval, heartbeatLEDblink );  

#if SERIAL_PRINT 
  Serial.println ( "... Blynk timers set up." );  
#endif  
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
#if SERIAL_PRINT 
  Serial.print ( "\nSetting up WLAN and Blynk with WiFiManager" );  
  Serial.println ( "Starting Blynk.begin" );  
#endif
  // SET UP THE CONFIG PORTAL CREDENTIALS AND CONNECTION INFO
  Blynk.setConfigPortalChannel ( 0 );  // 0 -> Use a random WiFi 2.4GHz channel for the Config Portal
  Blynk.setConfigPortal ( CONFIG_PORTAL_SSID, CONFIG_PORTAL_PASSWORD ); // Set up Config Portal SSID & Password
  Blynk.setConfigPortalIP ( IPAddress ( CONFIG_PORTAL_IPADDRESS ) ); // Set up IP address for Config Portal once connected to WiFi

#if SERIAL_PRINT 
  Serial.print ( "     " ); 
  Serial.print ( "Blynk.setConfigPortal(" ); 
  Serial.print ( CONFIG_PORTAL_SSID ); Serial.print ( "," );  
  Serial.print ( CONFIG_PORTAL_PASSWORD ); Serial.println ( ")" );   

  Serial.print ( "     " ); 
  Serial.print ( "Config Portal will be found at IP: " ); Serial.print ( IPAddress ( CONFIG_PORTAL_IPADDRESS ) );  
  Serial.println ( "\n" );  
#endif
    
  //Blynk.config ( blynkAuth );    // not needed with WM 
  //WiFi.mode ( WIFI_AP );  
  Blynk.begin ( BLYNK_HOST_NAME ); // DHCP (router) device name
  
#if SERIAL_PRINT 
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
#endif
} // end connectToWLANandBlynk





// BLYNK_CONNECTED GETS CALLED WHEN CONNECTING TO BLYNK SERVERS
// GETS CALLED IMMEDIATELY ON FIRST CONNECT TO BLYNK SERVER, TOO
BLYNK_CONNECTED()
{
#if SERIAL_PRINT 
  Serial.println ( "\nBLYNK_CONNECTED..." );  
#endif
  
  // This timer corrects a Blynk bug, allows all initial BLYNK_WRITEs to get flushed upon connect.
  //It's a ONE-TIME timer
  flushAtStartupTimerID = myTimer.setTimeout ( flushAtStartupTimerDuration, endFlushAtStartup );  

  // Set up the Blynk App with all the static info (colors and labels)
  initializeBlynkWidgets();

  // Set up the working Blynk App color and label info
  updateBlynkWidgets();
  
} // end BLYNK_CONNECTED




/* useless
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
*/



#if USE_LITTLEFS

void getLabelString()
{
#if SERIAL_PRINT 
  Serial.println ( "getLabelString: Getting Saved Label String from LittleFS" );  
#endif

  if ( ! LittleFS.begin() ) fileSystemBeginFailed();

  if ( LittleFS.exists ( DISPLAY_LABEL_FILE_NAME ) )
  {
#if SERIAL_PRINT 
    Serial.print ( "Retrieving the label name from LittleFS: " );  
#endif
    File labelFile = LittleFS.open ( DISPLAY_LABEL_FILE_NAME, "r" ); 
    String tempA = labelFile.readString(); 
    labelFile.close(); 
    tempA.toCharArray ( displayLabel, DISPLAY_LABEL_LENGTH+1 ); 
    
    Blynk.setProperty ( DISPLAY_VPIN,   "label",    displayLabel );    
  }
  else // No file exists. Crerate one. Set the default name
  {
#if SERIAL_PRINT 
    Serial.println ( "No file exists. Setting the default" );  
#endif
    File labelFile = LittleFS.open ( DISPLAY_LABEL_FILE_NAME, "w" ); 
    labelFile.print ( DEVICE_NAME ); 
    labelFile.close(); 
//  displayLabel[] = DEVICE_NAME; // set above in sketch
  }
#if SERIAL_PRINT 
  Serial.print ( "displayLabel = <" ); Serial.print ( displayLabel ); Serial.println ( ">\n" ); 
#endif
}



void saveLabelString()
{
#if SERIAL_PRINT 
  Serial.print ( "saveLabelString: Saving updated Label Name <" );  Serial.print ( displayLabel ); Serial.println ( ">\n" );  
#endif
  if ( ! LittleFS.begin() ) fileSystemBeginFailed();

  File labelFile = LittleFS.open ( DISPLAY_LABEL_FILE_NAME, "w" ); 
  labelFile.print ( displayLabel ); 
  labelFile.close(); 
}



void fileSystemBeginFailed()
{
#if SERIAL_PRINT 
  Serial.println ( "\n\n*************\n*************\n*************\n*************\n" ); 
  Serial.println ( "\n\n*************\n*************\n*************\n*************\n" ); 
  Serial.println ( "\n\n*************\n** LittleFS.begin() failed... formatting now. \n" ); 
  Serial.println ( "\n\n*************\n*************\n*************\n*************\n" ); 
  Serial.println ( "\n\n*************\n*************\n*************\n*************\n" ); 
#endif
  
  LittleFS.format();
}


#if SERIAL_PRINT
void listDir(const char * dirname) 
{
  Serial.printf ("\nListing FS directory: %s\n", dirname);
  if ( ! LittleFS.begin() ) fileSystemBeginFailed();

  Dir root = LittleFS.openDir(dirname);

  while (root.next()) {
    File file = root.openFile("r");
    Serial.print("  FILE: ");
    Serial.print(root.fileName());
    Serial.print("  SIZE: ");
    Serial.print(file.size());
    time_t cr = file.getCreationTime();
    time_t lw = file.getLastWrite();
    file.close();
    struct tm * tmstruct = localtime(&cr);
    Serial.printf("    CREATION: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    tmstruct = localtime(&lw);
    Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    Serial.println();  
  }
}
#endif
#endif // LittleFS




// EEPROM version
#if ! ( USE_LITTLEFS || USE_SPIFFS ) // EEPROM

#define MY_EEPROM_START 2000
#define EEPROM_FUNNY_CODE 243

void getLabelString()
{
#if SERIAL_PRINT 
  Serial.println ( "\ngetLabelString: Getting Saved Label String from EEPROM" );  
  printEEPROMstats(); 
#endif
  
  if ( EEPROM.read ( MY_EEPROM_START ) == EEPROM_FUNNY_CODE ) // Using a flag of one byte
  {
#if SERIAL_PRINT 
    Serial.print ( "\nFound the code. Retrieving the label name from EEPROM <" );  

    for ( int i = 0; i < DISPLAY_LABEL_LENGTH; i++ )
    {
      displayLabel[i] = EEPROM.read ( MY_EEPROM_START + 1 + i ); 
      Serial.print ( displayLabel[i] ); 
    }
    Serial.println ( ">" );
#endif
    // Update the label value from the default (coded) value
    Blynk.setProperty ( DISPLAY_VPIN,   "label",    displayLabel );    
  }
  else // No file exists. Create one. Set the default name
  {
#if SERIAL_PRINT 
    Serial.print ( "No file exists. Setting the default: " );  
#endif

    EEPROM.write ( MY_EEPROM_START, EEPROM_FUNNY_CODE ); // Put a code in to see if we've written successfully
    for ( int i = 0; i < DISPLAY_LABEL_LENGTH; i++ )
    {
      EEPROM.write ( MY_EEPROM_START + 1 + i, displayLabel[i] );
    }
    EEPROM.write ( MY_EEPROM_START + 1 + DISPLAY_LABEL_LENGTH , 0 ); // force ending of any char string
  }
  EEPROM.commit();  
   
#if SERIAL_PRINT
  Serial.println( "\nChecking after the write..." ); 
  printEEPROMstats(); 
#endif
}



void saveLabelString()
{
#if SERIAL_PRINT 
  Serial.print ( "\nsaveLabelString: Saving updated Label Name to EEPROM <" );  Serial.print ( displayLabel ); Serial.println ( ">\n" );  
  printEEPROMstats();
#endif

  EEPROM.write ( MY_EEPROM_START, EEPROM_FUNNY_CODE ); // Put a code in to see if we've written successfully
  
  for ( int i = 0; i < strlen ( displayLabel ); i++ )
  {
    EEPROM.write ( MY_EEPROM_START + 1 + i, displayLabel[i] );
  }
  EEPROM.write ( MY_EEPROM_START + 1 + strlen ( displayLabel ), 0 ); 
  EEPROM.commit();  
  
#if SERIAL_PRINT
  Serial.println( "\nChecking after the write..." );   
  printEEPROMstats();
#endif

}

#if SERIAL_PRINT
void printEEPROMstats()
{
  Serial.print ( "EEPROM length = " ); Serial.println ( EEPROM.length() );  
  Serial.print ( "EEPROM_START = " ); Serial.println ( EEPROM_START ); 
  Serial.print ( "BLYNK_EEPROM_START = " ); Serial.println ( BLYNK_EEPROM_START ); 
  
  Serial.print ( "MY_EEPROM_START = " ); Serial.println ( MY_EEPROM_START );  
  Serial.print ( "Reading from MY_EEPROM_START: " );  Serial.println ( EEPROM.read ( MY_EEPROM_START ) ); 
  
  Serial.print ( "Checking existing EEPROM copy <" );  
  for ( int i = 0; i < DISPLAY_LABEL_LENGTH; i++ )
  {
    if ( EEPROM.read ( MY_EEPROM_START + 1 + i ) == 0 ) break; 
    Serial.print ( char ( EEPROM.read ( MY_EEPROM_START + 1 + i ) ) ); 
  }
  Serial.println ( ">" ); 
  Serial.print ( "displayLabel <" ); Serial.print ( displayLabel ); Serial.println ( ">" );  
}
#endif


#endif
