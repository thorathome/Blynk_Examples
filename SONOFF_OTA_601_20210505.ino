#define SKETCH_NAME "SONOFF OTA 601"
#define SERIAL_SPEED 230400

#ifndef ESP8266
  #error Hey this is for SONOFF and ESP8266 ONLY!
#endif

/* README

The Arduino/Blynk sketch SONOFF_OTA_601_20210505.ino is a **fully-developed OTA-enabled, multi-device SONOFF control system** using the powerful BlynkSimpleESP8266_SSL_WM WiFiManager (WM) libraries for ESP8266. 

**If you are new to Arduino, Blynk, or SONOFFs, this probably is not the first Arduino sketch you should try.** It uses some advanced features and requires flashing a SONOFF. 

This sketch also demonstrates several Blynk, Blynk_WiFiManager, and Arduino IDE features.
* Basic Blynk_WiFiManager Config Portal configuration and use
* Wonderful OTA code from Andreas Spiess, The Guy With the Swiss Accent, from his YouTube video #332  _(I modified his code a bit to assume WiFi connection is already in place - set up by Blynk_WiFiManager. Modified code available in this repository. Thank you, Andreas Spiess.)_
* Use of multiple SONOFF devices on one Blynk Authcode
* Use of Blynk's BLYNK_WRITE_DEFAULT() flexible capability
* Use of compiler constants for compile-time device selection
* Use of EEPROM to remember the device labels in flash memory. _(LittleFS was too large for SONOFF's relatively small memory.)_

For each SONOFF device, the Blynk App wants to see...
* Blynk Button in SWITCH mode (ON/OFF)
* Blynk LED (Heartbeat)
* Blynk Value Display Widget (for the Switch Label)
* Blynk Text Input Widget (to set and display the Switch label... AND to force a Reboot)
* (Virtual Pins are hardcoded into the sketch, device by device, using #compiler directives.)
* **THEREFORE, for 6 SONOFF devices, you'll want 6 of each Widget.** _(I only have 1 Text Input Widget for all 6 SONOFFs, and I change the Vpin from the Blynk App when labelling or Rebooting the Switches.)_

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
   
This repository https://github.com/thorathome/Blynk_Examples 
   
See also https://github.com/khoih-prog/Blynk_WM for the Blynk WiFi Manager library. (You can download it directly from the IDE.)

See Andreas Spiess on YouTube for his OTA episodes. This one refers to ESP32, but is also applicable to ESP8266/SONOFF.
https://youtu.be/1pwqS_NUG7Q

*/



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



/// Need to use EEPROM due to size constraints of LittleFS (too large) in SONOFF (along with SSL)
/// Must have this BEFORE the #includes
#define USE_LITTLEFS      false    // true = LittleFS  false = EEPROM

#if USE_LITTLEFS
  #define DISPLAY_LABEL_FILE_NAME  "/SONOFF_DISPLAY.txt"
#endif



#define MY_SERIAL_PRINT_ON true    // All Serial.printxxx works as written
//#define MY_SERIAL_PRINT_ON false

#if MY_SERIAL_PRINT_ON
  #define BLYNK_PRINT       Serial // Generates Blynk debug prints. Comment out if not needed, saving space 
  #define BLYNK_WM_DEBUG    0      // 0 - 3
#endif
#include "MY_SERIAL_PRINTS.h"      // #defines mySerialP, mySerialPln, mySerialPfx, mySeriamBegin for Serial.print & ...ln if MY_SERIAL_PRINT is true, No-op if false



#include "MY_WIFI_CREDENTIALS.h"   // #defines MY_WIFI_SSID/2 AND MY_WIFI_PASSWORD/2
#include "MY_BLYNK_CREDENTIALS.h"  // #defines MY_BLYNK_SERVER and MY_xxx_AUTHCODE
#include "MY_ESP_LED_BUILTINS.h"   // #defines LED_BUILTIN_HIGH and _LOW for uniform use across devices
#include "MY_BLYNK_COLORS.h"       // #defines a bunch of handy Blynk colors



// ONLY ONE OF THESE CAN BE true
// I use several different devices, have named them from my childhood
#define LARRY       false   // 601 <===== updated 21 Apr 21 to new quasi-SSL WiFiManager 1.3.0
#define CURLY       false   // 601 <===== updated 21 Apr 21 to new quasi-SSL WiFiManager 1.3.0
#define MOE         false   // 601 <===== updated 21 Apr 21 to new quasi-SSL WiFiManager 1.3.0 
#define ROCKY       false   // 601 <===== updated 21 Apr 21 to new quasi-SSL WiFiManager 1.3.0
#define BORIS       false   // 601 <===== updated 21 Apr 21 to new quasi-SSL WiFiManager 1.3.0
#define NATASHA     false   // 601 <===== updated 21 Apr 21 to new quasi-SSL WiFiManager 1.3.0
#define SONOFF_DEV  true   // 601

// Look for more than one SONOFF, or none selected
#if   ( LARRY      && (          CURLY || MOE || ROCKY || BORIS || NATASHA || SONOFF_DEV ) ) // not only Larry, or
 ||   ( CURLY      && ( LARRY ||          MOE || ROCKY || BORIS || NATASHA || SONOFF_DEV ) ) // not only Curly, or
 ||   ( MOE        && ( LARRY || CURLY ||        ROCKY || BORIS || NATASHA || SONOFF_DEV ) ) // not only Moe, or
 ||   ( ROCKY      && ( LARRY || CURLY || MOE ||          BORIS || NATASHA || SONOFF_DEV ) ) // not only Rocky, or
 ||   ( BORIS      && ( LARRY || CURLY || MOE || ROCKY ||          NATASHA || SONOFF_DEV ) ) // not only Boris, or
 ||   ( NATASHA    && ( LARRY || CURLY || MOE || ROCKY || BORIS            || SONOFF_DEV ) ) // not only Natasha, or
 ||   ( SONOFF_DEV && ( LARRY || CURLY || MOE || ROCKY || BORIS || NATASHA               ) ) // not only SONOFF_DEV, or
 ||                 ! ( LARRY || CURLY || MOE || ROCKY || BORIS || NATASHA || SONOFF_DEV )   // none at all

  #error "ERROR1 Selecting Target SONOFF Platform"
#endif


// ALL SONOFF Devices use THE SAME Blynk Authcode!  (Development ESP8266 has different authcode.)
// Larry uses Virtual Pins 20-23, Curly is 30-33, Moe is 40-43, Boris is 50-53
// Development uses 10-13
#if LARRY
#define DEVICE_NAME            "SONOFF-LARRY"  // Choose device
#define CONTROL_VPIN           20
#define CONTROL_LABEL          "LARRY (INT.)"  // Shows up above ON/OFF button
#define HEARTBEAT_VPIN         21
#define HEARTBEAT_LABEL        "Working"
#define DISPLAY_VPIN           22
#define UPDATE_VPIN            23
#define CONFIG_PORTAL_SSID     "CONFIG_LARRY"  // IP and passwords set below
#define UPDATE_LABEL           "UPDATE LARRY"
#define MY_BLYNK_AUTHCODE      MY_REAL_SONOFF_AUTHCODE

#elif CURLY
#define DEVICE_NAME            "SONOFF-CURLY"  // Choose device
#define CONTROL_VPIN           30
#define CONTROL_LABEL          "CURLY (EXT.)"  // Shows up above ON/OFF button
#define HEARTBEAT_VPIN         31
#define HEARTBEAT_LABEL        "Working"
#define DISPLAY_VPIN           32
#define UPDATE_VPIN            33
#define UPDATE_LABEL           "UPDATE CURLY"
#define CONFIG_PORTAL_SSID     "CONFIG_CURLY"  // IP and passwords set below
#define MY_BLYNK_AUTHCODE      MY_REAL_SONOFF_AUTHCODE

#elif MOE
#define DEVICE_NAME            "SONOFF-MOE"    // Choose device
#define CONTROL_VPIN           40
#define CONTROL_LABEL          "MOE (EXT.)"    // Shows up above ON/OFF button
#define HEARTBEAT_VPIN         41
#define HEARTBEAT_LABEL        "Working"
#define DISPLAY_VPIN           42
#define UPDATE_VPIN            43
#define UPDATE_LABEL           "UPDATE MOE"
#define CONFIG_PORTAL_SSID     "CONFIG_MOE"    // IP and passwords set below
#define MY_BLYNK_AUTHCODE      MY_REAL_SONOFF_AUTHCODE

#elif ROCKY
#define DEVICE_NAME            "SONOFF-ROCKY"  // Choose device
#define CONTROL_VPIN           50
#define CONTROL_LABEL          "ROCKY (INT.)"  // Shows up above ON/OFF button
#define HEARTBEAT_VPIN         51
#define HEARTBEAT_LABEL        "Working"
#define DISPLAY_VPIN           52
#define UPDATE_VPIN            53
#define UPDATE_LABEL           "UPDATE ROCKY"
#define CONFIG_PORTAL_SSID     "CONFIG_ROCKY"  // IP and passwords set below
#define MY_BLYNK_AUTHCODE      MY_REAL_SONOFF_AUTHCODE

#elif BORIS
#define DEVICE_NAME            "SONOFF-BORIS"  // Choose device
#define CONTROL_VPIN           60
#define CONTROL_LABEL          "BORIS (INT.)"  // Shows up above ON/OFF button
#define HEARTBEAT_VPIN         61
#define HEARTBEAT_LABEL        "Working"
#define DISPLAY_VPIN           62
#define UPDATE_VPIN            63
#define UPDATE_LABEL           "UPDATE BORIS"
#define CONFIG_PORTAL_SSID     "CONFIG_BORIS"  // IP and passwords set below
#define MY_BLYNK_AUTHCODE      MY_REAL_SONOFF_AUTHCODE

#elif NATASHA
#define DEVICE_NAME            "SONOFF-NATASHA"  // Choose device
#define CONTROL_VPIN           70
#define CONTROL_LABEL          "NATASHA (INT.)"  // Shows up above ON/OFF button
#define HEARTBEAT_VPIN         71
#define HEARTBEAT_LABEL        "Working"
#define DISPLAY_VPIN           72
#define UPDATE_VPIN            73
#define UPDATE_LABEL           "UPDATE NATASHA"
#define CONFIG_PORTAL_SSID     "CONFIG_NATASHA"  // IP and passwords set below
#define MY_BLYNK_AUTHCODE      MY_REAL_SONOFF_AUTHCODE

#elif SONOFF_DEV
#define DEVICE_NAME            "SONOFF-DEV"    // Choose SONOFF_DEV development
#define CONTROL_VPIN           1
#define CONTROL_LABEL          "SONOFF (dev)"  // Shows up above ON/OFF button
#define HEARTBEAT_VPIN         2
#define HEARTBEAT_LABEL        "Working"
#define DISPLAY_VPIN           3
#define UPDATE_VPIN            4
#define UPDATE_LABEL           "UPDATE SONOFF_DEV"
#define CONFIG_PORTAL_SSID     "CONFIG_SONOFF_DEV" // IP and passwords set below
#define MY_BLYNK_AUTHCODE      MY_WORKING_DEVELOPMENT_AUTHCODE

#else
  #error "ERROR2 Selecting Target SONOFF Platform"
#endif

#if SONOFF_DEV
  #define SONOFF_RELAY_PIN     5               // ESP8266 RELAY ON/OFF
                                               // LED_BUILTIN for ESP8266 defined by MY_ESP_LED_BUILTINS.h
#else
  #define SONOFF_RELAY_PIN     12              // SONOFF RELAY ON/OFF
  #define LED_BUILTIN          13              // SONOFF ONBOARD LED
#endif



#define BLYNK_HOST_NAME DEVICE_NAME            // Shows up in Config Portal as Tab Name
// ALSO shows up in Router table as device name
#define OTA_BOARD_NAME  DEVICE_NAME            // used in OTA only



// Set up Blynk using Blynk_WiFiManager
#define USE_SSL true
#if USE_SSL
#include "MY_BLYNK_WIFIMANAGER_SETUP_SSL.h"     // SSL for Blynk not working on ESP8266 devices as of March 2021
#else                                             // Blynk_WiFiManager 1.3.0 (may have) fixed that (19 April 2021)
#include "MY_BLYNK_WIFIMANAGER_SETUP_NO_SSL.h"
#endif



// Cutting down on the brightness of the Blynk LED Widget
#define BLYNK_LED_WIDGET_MAX_BRIGHTNESS 150      // Led Widget is [0-255]  150 = not very bright (from 255)



// The Blynk Widgets display the device name AND a changable DisplayLabel.
// See UPDATE_VPIN and UPDATE_LABEL above
#define DISPLAY_LABEL_LENGTH                     17 // 16 + terminating 0
char    displayLabel[DISPLAY_LABEL_LENGTH]       = DEVICE_NAME;  



// Blynk Timers
// THIS SKETCH SETS UP A HEARTBEAT LED ON A TIMER TO SHOW SYSTEM IS ALIVE AND WELL
BlynkTimer myTimer;
// Blynk timers to blink a heartbeat LED on and off... ALSO checks RELAY STATUS
// NOTE: updateBlynkWidgets called by HEARTBEAT. updateBlynkWidgets takes 0.5 seconds to execute... every time
int heartbeatLEDinterval = 5000; // interval between heartbeats for onboard and Blynk Virtual LED in millisec
int heartbeatLEDtimerID;

int heartbeatLEDduration = 750; // duraction of each blink in millisec (set as an interval timer)
int heartbeatLEDdurationTimerID;
bool heartbeatLEDon = false; // this lets me use the same routine for the turn-on timer and the turn-off interval


// THIS TIMER IGNORES (AND RESTORES) BUTTON PUSHES WITHIN A SECOND OR SO
int ignoreQuickButtonPushesTimerDuration = 500;  // ignore quick button pushes for a half-second or so
int ignoreQuickButtonPushesTimerID;
bool ignoreQuickButtonPushes = false;


// AND ANOTHER QUICKIE TIMER TO LET US CHECK IF THE RELAY ACTUALLY SWITCHED AFTER A BUTTON PUSH
int verifyRelayTimerDuration = 995;  // verify the relay switched 1 second after button push
int verifyRelayTimerID;


// There's a bug in Blynk that allows all sorts of junk BLYNK_WRITE calls immediately on connecting to the app.
// This timer runs once upon connection and causes all new BLYNK_WRITEs to be ignored for a second or so
int flushAtStartupTimerDuration = 750; // 750 millisecs to flush everything out.
int flushAtStartupTimerID;
bool flushAtStartup = true; // FLUSH all BLYNK_WRITEs upon startup (Blynk Bug)



// This program is a finite state machine - FSM
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








//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SETUP WIFI, BLYNK, HEARTBEAT, AND RESTORE SONOFF SCHEDULES FROM EEPROM
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  // Never enough good printouts
  mySerialBegin ( SERIAL_SPEED );
  delay ( 500 );
  mySerialPln ( "\n\n=======================================" );
  mySerialP   ( SKETCH_NAME );
  mySerialP   ( " ** Using Blynk_WiFiManager and " );
#if USE_SSL
  mySerialPln ( "SSL **" );
#else
  mySerialPln ( "NOT Using SSL **" );
#endif


#if USE_LITTLEFS
  listDir ( "/" );
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


  // Set up Blynk and WiFi connection
  connectToWLANandBlynk();  // Connect to WiFi, then to Blynk server


  // Set up OTA
  setupOTA();


  // Read Label widget's deviceLabel from LittleFS or EEPROM
  getLabelString();


  // Blynk uses timers everywhere
  setupBlynkTimers();  // Establish Blynk timers after Blynk is connected


  // Make sure the relay is off (turnSONOFFoff uses a Blynk timer)
  turnSONOFFoff();  // this uses a timer, so timers must be set up above in setupBlynkTimers
  relayON = true; controlButtonON = false;  // trick checkRelay into executing fully and set the proper state


  // Set Blynk Virtual Heartbeat and LED_BUILTIN OFF
  heartbeatLEDon = false;
  digitalWrite ( LED_BUILTIN, LED_BUILTIN_LOW );
  Blynk.virtualWrite ( HEARTBEAT_VPIN, 000 );

  heartbeatLEDblink(); // start first heartbeat (uses timers, too)
  // This also calls checkRelay() and updateBlynkWidgets()
  

#if USE_LITTLEFS
  listDir ( "/" );
#endif


  mySerialPln ( "\nSetup complete \n*******************************\n*******************************\n" );

} //end setup





// KEEPING IT SIMPLE
void loop()
{
  Blynk.run();
  myTimer.run();

#if USE_OTA  // Do not use this code for ESP32 - find RTOS code
  ArduinoOTA.handle();
#endif
} // end loop





// Set up OTA for ESP8266 and ESP32 if requested by USE_OTA (Does nothing if not requested)
void setupOTA()
{
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
}





//   FINITE STATE MACHINE LAYOUT ( this diagram is duplicated below with checkRelay() )
//                 BLYNK_WRITE_DEFAULT()    checkRelay()
//   SYSTEM       | BUTTON     BUTTON  ||  RELAY      RELAY  |
//   STATE        |   ON    |   OFF    ||    ON    |   OFF   |
// -------------- | ------------------ || -------- | ------- |
// 1 BUTTON OFF   | -> 2    | (stay 1) || -> 6     |(stay 1) | steady state
//   RELAY  OFF   | TURN ON |          ||(trouble) |         |   OFF
//                |         |          ||          |         |
// 2 BUTTON ON    | (stay 2)| -> 4     || -> 3     |(stay 2) | waiting to
//   RELAY  OFF   | TURN ON | TURN OFF ||          |         | see relay ON
//                |         |          ||          |         |
// 3 BUTTON ON    | (stay 3)| -> 4     || (stay 3) | -> 6    | steady state
//   RELAY  ON    |         | TURN OFF ||          |(trouble)|   ON
//                |         |          ||          |         |
// 4 BUTTON OFF   | -> 2    | (stay 4) || (stay 4) | -> 1    | waiting to
//   RELAY  ON    | TURN ON | TURN OFF ||          |         | see relay OFF
//                |         |          ||          |         |
// 5 INITIALIZING | -> 2    | -> 4     || -> 4     | -> 1    | startup
//                | TURN ON | TURN OFF || TURN OFF |         |
//                |         |          ||          |         |
// 6 SYSTEM ERROR - restart the switch -
//

// BLYNK_WRITE_DEFAULT GETS CALLED WHEN THERE IS NO SPECIFIC BLYNK_WRITE FOR THAT VIRTUAL PIN
// This makes it a flexible - and programmable - receiver

// TAKE INPUTS FROM ON/OFF PUSHBUTTON or FLEXIBLE TEXT LABEL DISPLAY INPUT
BLYNK_WRITE_DEFAULT()
{
  // THE VIRTUAL PIN THAT SENT THE MESSAGE TO BLYNK
  int writeVpin = request.pin;

  mySerialP ( "\nBLYNK_WRITE_DEFAULT received a signal from V");
  mySerialP ( writeVpin );
  mySerialPln ( ": " );

  // Ugly but effective. take no action for the first second or so after Blynk connects
  if ( flushAtStartup )
  {
    mySerialP ( "     " ); mySerialPln ( "* flushed *" );
    return;  //////////////////////////////////////////////////////
  }

  // Print all parameter values for debugging
  for ( auto i = param.begin(); i < param.end(); ++i )
  {
    mySerialP ( "     " );
    mySerialP ( "* param = " ); mySerialPln ( i.asString() );
  }

  mySerialP ( "     " );
  mySerialPln ( "*****\n" );

  mySerialP ( "     " );
  mySerialP ( "Current systemState = " ); mySerialPln ( systemState );
  if ( protectSystemState ) mySerialPln ( "protectSystemState = TRUE" );


  // Check for everything screwed up
  if ( systemState < 1 || systemState > 5 )
  {
    mySerialPf1 ( "SYSTEMSTATE ERROR in BLYNK_WRITE_DEFAULT(Control Button Push). State = %i \n", systemState );
    dieAndReboot();
  }


  // TAKE ACTION ON BUTTON PUSH
  if ( writeVpin == CONTROL_VPIN ) // We have the ON/OFF input from Blynk App
  {
    if ( ! ignoreQuickButtonPushes ) // Ignore multile quick button pushes
    {
      // We just got a good Control Button Push so...
      // We will now attempt to change state

      // Protect this State Machine while it's running from timer-initiated routines also working the State machine
      if ( ! protectSystemState )
      {
        // We just got a good Control Button Push, and now will change state in the State Machine
        // Prevent other instances of the State Machine from changing state now
        protectSystemState = true;


        // Set a timer to IGNORE more quick buttonpushes for a short while
        ignoreQuickButtonPushes = true;
        ignoreQuickButtonPushesTimerID = myTimer.setTimeout ( ignoreQuickButtonPushesTimerDuration, ignoreQuickButtonPushesStop );


        // Get the ON-Off button value, turn the Sonoff ON or OFF
        int controlButtonValue = param[0].asInt();
        mySerialP ( "     " );
        mySerialP ( "Control Button Hit with value = " ); mySerialPln ( controlButtonValue );

        if ( controlButtonValue == 1 )
          controlButtonON = true;
        else
          controlButtonON = false;


        if ( controlButtonON )
          switch ( systemState )
          {
            case BUTTON_OFF_RELAY_OFF: // 1
              {
                systemState = BUTTON_ON_WAITING_FOR_RELAY; // -> 2
                turnSONOFFon();
                break;
              }

            case BUTTON_ON_WAITING_FOR_RELAY: // 2
              {
                systemState = BUTTON_ON_WAITING_FOR_RELAY; // (stay 2)
                turnSONOFFon();
                break;
              }

            case BUTTON_ON_RELAY_ON: // 3
              {
                systemState = BUTTON_ON_RELAY_ON; // (stay 3)
                break;
              }

            case BUTTON_OFF_WAITING_FOR_RELAY: // 4
              {
                systemState = BUTTON_ON_WAITING_FOR_RELAY; // -> 2
                turnSONOFFon();
                break;
              }

            case INITIALIZING: // 5
              {
                systemState = BUTTON_ON_WAITING_FOR_RELAY; // -> 2
                turnSONOFFon();
                break;
              }
          } // end Control Button ON switch/case stmt

        else // Control Button is OFF
          switch ( systemState )
          {
            case BUTTON_OFF_RELAY_OFF: // 1
              {
                systemState = BUTTON_OFF_RELAY_OFF; // (stay)
                break;
              }

            case BUTTON_ON_WAITING_FOR_RELAY: // 2
              {
                systemState = BUTTON_OFF_WAITING_FOR_RELAY; // -> 4
                turnSONOFFoff();
                break;
              }

            case BUTTON_ON_RELAY_ON: // 3
              {
                systemState = BUTTON_OFF_WAITING_FOR_RELAY; // -> 4
                turnSONOFFoff();
                break;
              }

            case BUTTON_OFF_WAITING_FOR_RELAY: // 4
              {
                systemState = BUTTON_OFF_WAITING_FOR_RELAY; // (stay)
                turnSONOFFoff();
                break;
              }

            case INITIALIZING: // 5
              {
                systemState = BUTTON_OFF_WAITING_FOR_RELAY; // -> 4
                turnSONOFFoff();
                break;
              }

          } // end Good Button Push case stmt

        // We are done working the FSM now, so can open it up for other instances
        protectSystemState = false;

        // Update the Blynk App widgets to the currentState
        updateBlynkWidgets();

      } // end protection for FSM changes on GOOD button push

    } // end non-ignored Control Button Push (ignored/non-ignored)

  } // end of processing any Control Button Push (ignored/not)

  else if ( writeVpin == UPDATE_VPIN ) // We have an hit on the 'invisible' update labels button
  {
    // Get the Update Label value, Hit Reset and reboot if = "RESET"
    String buttonVal = param[0].asStr();

    mySerialP ( "     " );
    mySerialP ( "UPDATE LABEL INPUT with value  = " ); mySerialPln ( buttonVal );
    if ( buttonVal == "RESET" || buttonVal == "Reset" || buttonVal == "Reboot" )
    {
      mySerialPln ( " \n REBOOTING " );
      dieAndReboot();
    }
    else
    {
      // We have a new displayLabel value to set
      buttonVal.toCharArray ( displayLabel, DISPLAY_LABEL_LENGTH );
      mySerialP ( "UPDATE LABEL OUTPUT with value = " ); mySerialPln ( displayLabel );

      // Save updated Label in LittleFS or EEPROM for next time
      saveLabelString();

      // Now update the Blynk Widgets
      Blynk.setProperty ( DISPLAY_VPIN,   "label",    displayLabel );

      // Update the Blynk App widget with new name
      updateBlynkWidgets();
    }
  }

  mySerialP ( "     " );
  mySerialP ( "New systemState = " ); mySerialPln ( systemState );

  mySerialPln ( "exited BLYNK_WRITE_DEFAULT" );

} //end BLYNK_WRITE_DEFAULT





//   FINITE STATE MACHINE LAYOUT ( this diagram is duplicated above BLYNK_WRITE_DEFAULT )
//                 BLYNK_WRITE_DEFAULT()    checkRelay()
//   SYSTEM       | BUTTON     BUTTON  ||  RELAY      RELAY  |
//   STATE        |   ON    |   OFF    ||    ON    |   OFF   |
// -------------- | ------------------ || -------- | ------- |
// 1 BUTTON OFF   | -> 2    | (stay 1) || -> 6     |(stay 1) | steady state
//   RELAY  OFF   | TURN ON |          ||(trouble) |         |   OFF
//                |         |          ||          |         |
// 2 BUTTON ON    | (stay 2)| -> 4     || -> 3     |(stay 2) | waiting to
//   RELAY  OFF   | TURN ON | TURN OFF ||          |         | see relay ON
//                |         |          ||          |         |
// 3 BUTTON ON    | (stay 3)| -> 4     || (stay 3) | -> 6    | steady state
//   RELAY  ON    |         | TURN OFF ||          |(trouble)|   ON
//                |         |          ||          |         |
// 4 BUTTON OFF   | -> 2    | (stay 4) || (stay 4) | -> 1    | waiting to
//   RELAY  ON    | TURN ON | TURN OFF ||          |         | see relay OFF
//                |         |          ||          |         |
// 5 INITIALIZING | -> 2    | -> 4     || -> 4     | -> 1    | startup
//                | TURN ON | TURN OFF || TURN OFF |         |
//                |         |          ||          |         |
// 6 SYSTEM ERROR - restart the switch -
//

// CHECK TO SEE IF THE RELAY STATE MATCHES THE CONTROL
// Runs on timers and from heartbeat
void checkRelay()
{
  mySerialP ( "\n    checkRelay called: " );

  bool newRelayON;
#if ! SONOFF_DEV
  if ( digitalRead ( SONOFF_RELAY_PIN ) == 1 )
    newRelayON = true;
  else
    newRelayON = false;
#else
  newRelayON = controlButtonON;  // Match the button while developing without an actual SONOFF
#endif

  mySerialPf1 ( " relayON = %i ", relayON );
  mySerialPf1 ( ", controlButtonON = %i \n", controlButtonON );

  mySerialPf1 ( "        Current systemState = %i \n", systemState );


  // Check for everything screwed up
  if ( systemState < 1 || systemState > 5 )
  {
    mySerialPf1 ( "SYSTEMSTATE ERROR in updateBlynkWidgets. State = %i \n", systemState );
    dieAndReboot();
  }


  if ( newRelayON == relayON )
  {
    mySerialP   ( "        No change in relay status. systemState = " ); mySerialPln ( systemState );
  }
  else  // relay has changed state - now change systemState if needed
  {
    relayON = newRelayON;

    // Relay state has changed, so we will change systemState
    // Prevent this from overwriting another change to the systemState elsewhere
    if ( ! protectSystemState )
    {
      // Don't let anyone else mess with the FSM during this process
      protectSystemState = true;

      if ( relayON )
        switch ( systemState )
        {
          case BUTTON_OFF_RELAY_OFF:         // 1
            {
              systemState = SYSTEM_ERROR; // -> 6 (trouble)
              break;
            }

          case BUTTON_ON_WAITING_FOR_RELAY:  // 2
            {
              systemState = BUTTON_ON_RELAY_ON; // -> 3
              break;
            }

          case BUTTON_ON_RELAY_ON:           // 3
            {
              systemState = BUTTON_ON_RELAY_ON; // (stay)
              break;
            }

          case BUTTON_OFF_WAITING_FOR_RELAY: // 4
            {
              systemState = BUTTON_OFF_WAITING_FOR_RELAY; // -> (stay)
              break;
            }

          case INITIALIZING:                 // 5
            {
              systemState = BUTTON_OFF_WAITING_FOR_RELAY; // -> 4
              turnSONOFFoff();
              break;
            }
        } // end relay = ON switch/case

      else // relay is OFF
        switch ( systemState )
        {
          case BUTTON_OFF_RELAY_OFF:         // 1
            {
              systemState = BUTTON_OFF_RELAY_OFF; // (stay)
              break;
            }

          case BUTTON_ON_WAITING_FOR_RELAY:  // 2
            {
              systemState = BUTTON_ON_WAITING_FOR_RELAY; // (stay)
              break;
            }

          case BUTTON_ON_RELAY_ON:           // 3
            {
              systemState = SYSTEM_ERROR; // ->6 (trouble)
              break;
            }

          case BUTTON_OFF_WAITING_FOR_RELAY: // 4
            {
              systemState = BUTTON_OFF_RELAY_OFF; // -> 1
              break;
            }

          case INITIALIZING:                 // 5
            {
              systemState = BUTTON_OFF_RELAY_OFF; // -> 1
              break;
            }
        } // end relay is OFF switch/case statement

      // We ran in a protectedSystemState = true mode while FSM was running
      protectSystemState = false;

    } // end of protectSystemsState

    mySerialP   ( "        New systemState = " ); mySerialPln ( systemState );

  } // end relay-changed-status

  mySerialPln ( "    exited checkRelay" );

} // end checkRelay





//   FINITE STATE MACHINE DISPLAY LAYOUT
//
//   SYSTEM       |
//   STATE        | controlColor | controlWrite | heartbeatColor |  statusDisplay   |
// -------------- |------------- | ------------ | -------------- | ---------------- |
// 1 BUTTON OFF   | BLYNK_GREEN  |     LOW      |  BLYNK_GREEN   |  " is OFF"       |
//   RELAY  OFF   |              |              |                |                  |
//                |              |              |                |                  |
// 2 BUTTON ON    | BLYNK_RED    |     HIGH     |  BLYNK_GREEN   |  " turning ON"   |
//   RELAY  OFF   |              |              |                |                  |
//                |              |              |                |                  |
// 3 BUTTON ON    | BLYNK_RED    |     HIGH     |  BLYNK_RED     |  " is ON"        |
//   RELAY  ON    |              |              |                |                  |
//                |              |              |                |                  |
// 4 BUTTON OFF   | BLYNK_GREEN  |     LOW      |  BLYNK_RED     |  " turning OFF"  |
//   RELAY  ON    |              |              |                |                  |
//                |              |              |                |                  |
// 5 INITIALIZING | BLYNK_YELLOW |     LOW      |  BLYNK_GREY    |  " initializing" |
//                |              |              |                |                  |
//                |              |              |                |                  |
// 6 SYSTEM ERROR - unused here
//

// Set up the operating colors and labels
void updateBlynkWidgets()
{
  String controlButtonColor = BLYNK_WHITE;;
  String heartbeatColor = BLYNK_WHITE;
  int controlButtonWrite = -1;
  String statusDisplay = "";

  mySerialP ( "            " );
  mySerialP ( "updateBlynkWidgets called with systemState " );
  mySerialPln ( systemState );

  if ( systemState < 1 || systemState > 5 )
  {
    mySerialPf1 ( "SYSTEMSTATE ERROR in updateBlynkWidgets %i \n", systemState );
    dieAndReboot();
  }

  // Do not let Button-triggered or timer-triggered changes to systemState screw up this routine
  if ( protectSystemState )
  {
    mySerialPln ( "  >> updateBlynkWidgets called while protectedSystemState = true" );
  }

  else // OK to execute on the BlynkApp updates
  {
    protectSystemState = true;

    switch ( systemState )
    {
      case BUTTON_OFF_RELAY_OFF:         // 1
        controlButtonColor = BLYNK_GREEN;
        heartbeatColor = BLYNK_GREEN;
        controlButtonWrite = 0;
        statusDisplay = " is OFF";
        break;

      case BUTTON_ON_WAITING_FOR_RELAY:  // 2
        controlButtonColor = BLYNK_RED;
        heartbeatColor = BLYNK_GREEN;
        controlButtonWrite = 1;
        statusDisplay = " turning ON";
        break;

      case BUTTON_ON_RELAY_ON:           // 3
        controlButtonColor = BLYNK_RED;
        heartbeatColor = BLYNK_RED;
        controlButtonWrite = 1;
        statusDisplay = " is ON";
        break;

      case BUTTON_OFF_WAITING_FOR_RELAY: // 4
        controlButtonColor = BLYNK_GREEN;
        heartbeatColor = BLYNK_RED;
        controlButtonWrite = 0;
        statusDisplay = " turning OFF";
        break;

      case INITIALIZING:                 // 5
        controlButtonColor = BLYNK_YELLOW;
        heartbeatColor = BLYNK_GREY;
        controlButtonWrite = 0;
        statusDisplay = " Initializing";
        break;
    }

    Blynk.setProperty  ( CONTROL_VPIN, "color", controlButtonColor );
    Blynk.virtualWrite ( CONTROL_VPIN, controlButtonWrite );

    Blynk.setProperty  ( HEARTBEAT_VPIN, "color", heartbeatColor );

    //statusDisplay = String ( displayLabel ) + statusDisplay;
    Blynk.virtualWrite ( DISPLAY_VPIN, String ( displayLabel ) + statusDisplay );


    protectSystemState = false;

  } // end of protectedSystemState = false

  mySerialP ( "            " );
  mySerialPln( "exited updateBlynkWidgets " );

} // end updateBlynkWidgets





// This flag lets us flush unwanted BLYNK_WRITE_DEFAULT calls on initial connections (Blynk bug)
// Thsi is called by a timer after connection
void ignoreQuickButtonPushesStop()
{
  ignoreQuickButtonPushes = false;
}





// TURN ON THE SONOFF SWITCH
//  MAKE THE BUTTON AND HEARTBEAT LED RED
//  MAKE THE DISPLAY SAY UNIT IS ON
void turnSONOFFon()
{
  mySerialPln ( "\n***********************" );
  mySerialPln (    "** TURNING SONOFF ON **" );
  mySerialPln (    "***********************\n" );
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
  mySerialPln ( "\n************************" );
  mySerialPln (    "** TURNING SONOFF OFF **" );
  mySerialPln (    "************************\n" );
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
  mySerialPln ( "\ninitializeBlynkWidgets called: " );
  mySerialP ( CONTROL_VPIN );   mySerialP ( ", " );
  mySerialP ( HEARTBEAT_VPIN ); mySerialP ( ", " );
  mySerialP ( DISPLAY_VPIN );   mySerialP ( ", " );
  mySerialP ( UPDATE_VPIN );    mySerialPln();

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

  mySerialPln ( "initializeBlynkWidgets completed" );

} // end initializeBlynkWidgets





// DIE UNGRACEFULLY - ASSUME DIAGNOSTIC PRINT FROM SOURCE - NOT HERE
void dieAndReboot()
{
  mySerialPln ( "\nSYSTEM ERROR" );
  mySerialPln (   "SYSTEM ERROR" );
  mySerialPln (   "SYSTEM ERROR" );
  mySerialPln (   "SYSTEM ERROR\n" );

  ////////////////////////////////////////////////////////////////////////
  // RESTART SYSTEM ON systemState ERROR
  ////////////////////////////////////////////////////////////////////////
  delay ( 2000 );
  ESP.restart();

} // end dieAndReboot





// LED HEARTBEAT and checkRelay() and updateBlynkWidgwewt
void heartbeatLEDblink()
/* Blink the on-board LED AND the Virtual Blynk LED
   If LED is off, it turne the LEDs on, then sets a timer to turn LED off
   When the timer triggers, same routine turns the LEDs off
*/
{
  if ( heartbeatLEDon )
  {
    digitalWrite ( LED_BUILTIN, LED_BUILTIN_LOW ); // On-board LED off
    Blynk.virtualWrite ( HEARTBEAT_VPIN, 000 );     // Blynk LED off

    mySerialPln ( " ..." );
#if USE_TELNET
    TelnetStream.println ( " ..." );
#endif
  }

  else // LED is OFF. Turn it ON... AND CHECK RELAY
  {
    // LED is OFF - turn it on, check the relay, set a timer to turn heartbeat back off
    mySerialP ( "\n... heartbeat of " ); mySerialP ( DEVICE_NAME );
#if USE_SSL
    mySerialP ( " SSL" );
#else
    mySerialP ( " NO SSL" );
#endif
    mySerialP ( " WiFi.status() = " ); mySerialP ( WiFi.status() );


#if USE_TELNET
    TelnetStream.print ( "\n... heartbeat of " ); TelnetStream.print ( DEVICE_NAME );
#if USE_SSL
    TelnetStream.print ( " SSL" );
#else
    TelnetStream.print ( " NO SSL" );
#endif
    TelnetStream.print ( " WiFi.status() = " ); TelnetStream.print ( WiFi.status() );
#endif

    analogWrite ( LED_BUILTIN, 750 );                                       // Turn On-board LED on
    Blynk.virtualWrite ( HEARTBEAT_VPIN, BLYNK_LED_WIDGET_MAX_BRIGHTNESS ); // Blynk LED on


    // See if relay state has changed
    checkRelay();  // check to see relay status and change systemState if needed
    // checkRelay calls updateBlynkWidgets ONLY ON A CHANGE OF STATE


    // Update 4 Blynk Widgets from systemState with each heartbeat
    updateBlynkWidgets();  


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

  mySerialPln ( "Setting up Blynk timers" );
  mySerialP   ( "... HeartbeartLEDinterval = " ); mySerialPln ( randomizedHeartbeatLEDinterval );

  heartbeatLEDtimerID = myTimer.setInterval ( randomizedHeartbeatLEDinterval, heartbeatLEDblink );

  mySerialPln ( "... Blynk timers set up." );

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
  mySerialP ( "\nSetting up WLAN and Blynk with WiFiManager" );
  mySerialPln ( "Starting Blynk.begin" );
  // SET UP THE CONFIG PORTAL CREDENTIALS AND CONNECTION INFO
  Blynk.setConfigPortalChannel ( 0 );  // 0 -> Use a random WiFi 2.4GHz channel for the Config Portal
  Blynk.setConfigPortal ( CONFIG_PORTAL_SSID, CONFIG_PORTAL_PASSWORD ); // Set up Config Portal SSID & Password
  Blynk.setConfigPortalIP ( IPAddress ( CONFIG_PORTAL_IPADDRESS ) ); // Set up IP address for Config Portal once connected to WiFi

  mySerialP ( "     " );
  mySerialP ( "Blynk.setConfigPortal(" );
  mySerialP ( CONFIG_PORTAL_SSID ); mySerialP ( "," );
  mySerialP ( CONFIG_PORTAL_PASSWORD ); mySerialPln ( ")" );

  mySerialP ( "     " );
  mySerialP ( "Config Portal will be found at IP: " ); mySerialP ( IPAddress ( CONFIG_PORTAL_IPADDRESS ) );
  mySerialPln ( "\n" );

  //Blynk.config ( blynkAuth );    // not needed with WM
  //WiFi.mode ( WIFI_AP );
  Blynk.begin ( BLYNK_HOST_NAME ); // DHCP (router) device name

  if ( Blynk.connected() )
  {
#if ( USE_LITTLEFS || USE_SPIFFS)
    mySerialPln ( "\nBlynk using " + String ( CurrentFileFS ) + " connected. Board Name : " + Blynk.getBoardName() );
#else
    mySerialPln ( "\nBlynk using EEPROM connected. Board Name : " + Blynk.getBoardName() );
    mySerialPf3 ( "EEPROM size = %d bytes, EEPROM start address = %d / 0x%X\n", EEPROM_SIZE, EEPROM_START, EEPROM_START );
#endif

    mySerialPln ( "Blynk connected just fine" );
    mySerialP   ( "  IP address  " ); mySerialPln ( WiFi.localIP() ) ;
    mySerialP   ( "  MAC address " ); mySerialPln ( WiFi.macAddress() );
    mySerialPln();
  }
  else mySerialPln ( "Blynk NOT CONNECTED \n\n" );

} // end connectToWLANandBlynk





// BLYNK_CONNECTED GETS CALLED WHEN CONNECTING TO BLYNK SERVERS
// GETS CALLED IMMEDIATELY ON FIRST CONNECT TO BLYNK SERVER, TOO
BLYNK_CONNECTED()
{
  mySerialPln ( "\nBLYNK_CONNECTED..." );

  // This timer corrects a Blynk bug, allows all initial BLYNK_WRITEs to get flushed upon connect.
  //It's a ONE-TIME timer
  flushAtStartupTimerID = myTimer.setTimeout ( flushAtStartupTimerDuration, endFlushAtStartup );

  // Set up the Blynk App with all the static info (colors and labels)
  initializeBlynkWidgets();

  // Set up the working Blynk App color and label info
  updateBlynkWidgets();

} // end BLYNK_CONNECTED





// BLYNK_APP_CONNECTED GETS CALLED WHEN APP CONNECTS TO BLYNK SERVERS
// IT IS NOT SUPER RELIABLE !
BLYNK_APP_CONNECTED()
{
  mySerialPln ( "\nBLYNK_APP_CONNECTED..." );

  // Set up the Blynk App with all the static info (colors and labels)
  initializeBlynkWidgets();

  // Set up the working Blynk App color and label info
  updateBlynkWidgets();

} // end BLYNK_APP_CONNECTED





#if USE_LITTLEFS

void getLabelString()
{
  mySerialPln ( "getLabelString: Getting Saved Label String from LittleFS" );

  if ( ! LittleFS.begin() ) fileSystemBeginFailed();

  if ( LittleFS.exists ( DISPLAY_LABEL_FILE_NAME ) )
  {
    mySerialP ( "Retrieving the label name from LittleFS: " );
    File labelFile = LittleFS.open ( DISPLAY_LABEL_FILE_NAME, "r" );
    String tempA = labelFile.readString();
    labelFile.close();
    tempA.toCharArray ( displayLabel, DISPLAY_LABEL_LENGTH );

    Blynk.setProperty ( DISPLAY_VPIN,   "label",    displayLabel );
  }
  else // No file exists. Crerate one. Set the default name
  {
    mySerialPln ( "No file exists. Setting the default" );

    File labelFile = LittleFS.open ( DISPLAY_LABEL_FILE_NAME, "w" );
    labelFile.print ( DEVICE_NAME );
    labelFile.close();
    //  displayLabel[] = DEVICE_NAME; // set above in sketch
  }
  mySerialP ( "displayLabel = <" ); mySerialP ( displayLabel ); mySerialPln ( ">\n" );

} // end getLabelString


// LittleFS code

void saveLabelString()
{
  mySerialP ( "saveLabelString: Saving updated Label Name <" );  mySerialP ( displayLabel ); mySerialPln ( ">\n" );

  if ( ! LittleFS.begin() ) fileSystemBeginFailed();

  File labelFile = LittleFS.open ( DISPLAY_LABEL_FILE_NAME, "w" );
  labelFile.print ( displayLabel );
  labelFile.close();
}


// LittleFS code

void fileSystemBeginFailed()
{
  mySerialPln ( "\n\n*************\n*************\n*************\n*************\n" );
  mySerialPln ( "\n\n*************\n*************\n*************\n*************\n" );
  mySerialPln ( "\n\n*************\n** LittleFS.begin() failed... formatting now. \n" );
  mySerialPln ( "\n\n*************\n*************\n*************\n*************\n" );
  mySerialPln ( "\n\n*************\n*************\n*************\n*************\n" );

  LittleFS.format();
}


// LittleFS code

void listDir(const char * dirname)
{
  mySerialPf1 ("\nListing FS directory: %s\n", dirname);
  if ( ! LittleFS.begin() ) fileSystemBeginFailed();

  Dir root = LittleFS.openDir(dirname);

  while (root.next()) {
    File file = root.openFile("r");
    mySerialP("  FILE: ");
    mySerialP(root.fileName());
    mySerialP("  SIZE: ");
    mySerialP(file.size());
    time_t cr = file.getCreationTime();
    time_t lw = file.getLastWrite();
    file.close();
    struct tm * tmstruct = localtime(&cr);
    mySerialPf6("    CREATION: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    tmstruct = localtime(&lw);
    mySerialPf6("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    mySerialPln();
  }
}
#endif // LittleFS




// EEPROM version
#if ! ( USE_LITTLEFS || USE_SPIFFS ) // EEPROM code follows

#define MY_EEPROM_START 2000
#define EEPROM_FUNNY_CODE 243

void getLabelString()
{
  mySerialPln ( "\ngetLabelString: Getting Saved Label String from EEPROM" );
  printEEPROMstats();


  if ( EEPROM.read ( MY_EEPROM_START ) == EEPROM_FUNNY_CODE ) // Using a flag of one byte
  {
    mySerialP ( "\nFound the code. Retrieving the label name from EEPROM <" );

    for ( int i = 0; i < DISPLAY_LABEL_LENGTH; i++ )
    {
      displayLabel[i] = EEPROM.read ( MY_EEPROM_START + 1 + i ); if ( displayLabel[i] == 0 ) break;
      mySerialP ( displayLabel[i] );
    }
    mySerialPln ( ">" );

    // Update the label value from the default (coded) value
    mySerialP   ( "Saved displayLabel as <" ); mySerialP ( displayLabel ); mySerialPln ( ">" );
    Blynk.setProperty ( DISPLAY_VPIN,   "label",    displayLabel );
  }
  else // No file exists. Create one. Set the default name
  {
    mySerialP ( "No file exists. Setting the default: " );

    EEPROM.write ( MY_EEPROM_START, EEPROM_FUNNY_CODE ); // Put a code in to see if we've written successfully
    for ( int i = 0; i < DISPLAY_LABEL_LENGTH; i++ )
    {
      EEPROM.write ( MY_EEPROM_START + 1 + i, displayLabel[i] ); if ( displayLabel[i] == 0 ) break;
    }
  }
  EEPROM.commit();

  mySerialPln( "\nChecking after the write..." );
  printEEPROMstats();

} // end getLabelString


// EEPROM code

void saveLabelString()
{
  mySerialP ( "\nsaveLabelString: Saving updated Label Name to EEPROM <" );  mySerialP ( displayLabel ); mySerialPln ( ">\n" );
  printEEPROMstats();

  EEPROM.write ( MY_EEPROM_START, EEPROM_FUNNY_CODE ); // Put a code in to see if we've written successfully

  for ( int i = 0; i < strlen ( displayLabel ); i++ )
  {
    EEPROM.write ( MY_EEPROM_START + 1 + i, displayLabel[i] );
  }
  EEPROM.write ( MY_EEPROM_START + 1 + strlen ( displayLabel ), 0 );
  EEPROM.commit();

  mySerialPln( "\nChecking after the write..." );
  printEEPROMstats();

} // end saveLabelString


// EEPROM code

void printEEPROMstats()
{
  mySerialP ( "EEPROM length = " ); mySerialPln ( EEPROM.length() );
  mySerialP ( "EEPROM_START = " ); mySerialPln ( EEPROM_START );
  mySerialP ( "BLYNK_EEPROM_START = " ); mySerialPln ( BLYNK_EEPROM_START );

  mySerialP ( "MY_EEPROM_START = " ); mySerialPln ( MY_EEPROM_START );
  mySerialP ( "Reading from MY_EEPROM_START: " );  mySerialPln ( EEPROM.read ( MY_EEPROM_START ) );

  mySerialP ( "Checking existing EEPROM copy <" );
  for ( int i = 0; i < DISPLAY_LABEL_LENGTH; i++ )
  {
    if ( EEPROM.read ( MY_EEPROM_START + 1 + i ) == 0 ) break;
    mySerialP ( char ( EEPROM.read ( MY_EEPROM_START + 1 + i ) ) );
  }
  mySerialPln ( ">\n" );
}
#endif // EEPROM code
