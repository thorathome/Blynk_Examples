/*
Blynkers. I am porting my ESP32/8266 Blynk apps to the new Blynk, learning as I go.

However, I have used **Blynk's Webhook widgets** to POST measurements and operating info to IFTTT for observation, debugging and data analysis.  (The ESP32/8266 did a virtualWrite to a Webhook widget, which then POSTed the info to an IFTTT Applet.) I have an IFTTT account with several IFTTT Applets that receive POSTs from Blynk Webhook widgets. Most then write that data to Google Sheets. **It's a great way to analyze data produced by the ESPs, AND to debug OTA devices running in the wild.**  

The new Blynk does not (yet) support the Webhook widget. 
So what to do? 

I found this code from **@Blynk_Coeur** in our Blynk Community, which got me started. Thanks **@Blynk_Coeur**. 
[https://community.blynk.cc/t/webhook-widget/54152/9?u=thorathome](https://community.blynk.cc/t/webhook-widget/54152/9?u=thorathome)
Then I found a **great tutorial on ESP32 / 8266 HTTP GET and POST.** 
[https://randomnerdtutorials.com/esp32-http-get-post-arduino/#http-post](https://randomnerdtutorials.com/esp32-http-get-post-arduino/#http-post)

I wrote a short sketch to **POST to IFTTT using text/plain strings OR by using JSON**, which is more powerful. Both options are in the code, below. JSON POSTs allow greater flexibility, especially using character strings, and the code takes up no more memory. I did not find much difference between using text/plain and application/x-www-form-urlencoded Content-type.

I still miss being able to have an IFTTT Applet send data to a Blynk Webhooks Widget or directly to an ESP32/8266. So if anyone has figured that one out, please post. 

* The `postToIFTTT()` sketch assumes you have a WiFi or Ethernet connection up and running, with or without Blynk.
* It is presented as a self-contained subroutine using no global variables. 
* It includes one additional library **HTTPClient.h**. 
* While specific to IFTTT, this can be a starting point for any HTTP POST to an API.  

**You will want an IFTTT account.** Start with free. Even their PRO version is not expensive. 
Under IFTTT's Webhooks documentation is your IFTTT KEY with some simple instructions. 
**You will need the IFTTT KEY** for your POST (and GET) code.  
**You will also need the name of your IFTTT Applet** which consumes the POST and does something with it. (I love Google Sheets!)

I hope it's helpful, have also posted this code on my Github. 
*/



#define USE_JSON_FOR_IFTTT true
//#define USE_JSON_FOR_IFTTT false

#include <HTTPClient.h>
#define HTTP_CODE_OK 200  // Good HTTP return code

#include "MY_IFTTT_CREDENTIALS.h"           

//#define MY_IFTTT_KEY    "included"           // from Webhooks doc in IFTTT when you have an IFTTT account
#define MY_IFTTT_APPLET MY_IFTTT_SONOFF_APPLET // from IFTTT Applet which consumes the webhook POST

// Very helpful article on HTTP GET and POST 
// https://randomnerdtutorials.com/esp32-http-get-post-arduino/#http-post

void postToIFTTT ( const String& message ) 
{
  Serial.println ( "\npostToIFTTT called with message <" + message + ">" ); 

  // Set up an HTTP client object for the IFTTT POST 
  HTTPClient IFTTTpost;  
  
  // Server name URL

  String IFTTTserverName = "https://maker.ifttt.com/trigger/" + String ( MY_IFTTT_APPLET ) + "/with/key/" + String ( MY_IFTTT_KEY );

  long elapsed = millis();  
  
#if ESP32
  IFTTTpost.begin ( IFTTTserverName );  //Specify request destination, open HTTP connecction
#elif ESP8266
  WiFiClientSecure client;
  IFTTTpost.begin ( client, IFTTTserverName );  //Specify request destination, open HTTP connecction
#endif

  Serial.print ( " IFTTT ServerName <" ); Serial.print ( IFTTTserverName ); Serial.println ( ">" ); 

#if USE_JSON_FOR_IFTTT
  IFTTTpost.addHeader ( "Content-Type", "application/json" );

  // IFTTT's JSON payload is { "value1" : "xx", "value2" : "mm", "value3" : "gg" }
  // IFTTTrequest = "{ \"value1\" : \"Yo_Bud\", \"value2\" : \"mm\", \"value3\" : \"gg" }";
  String IFTTTrequest = "{ \"value1\" : \"" + String ( message ) + "\" }"; // spaces permitted !
  Serial.print ( "  JSON POST is    <" ); Serial.print ( IFTTTrequest ); Serial.println ( ">" ); 

#else
  //IFTTTpost.addHeader ( "Content-Type", "text/plain" );
  IFTTTpost.addHeader ( "Content-Type", "application/x-www-form-urlencoded" );

  // IFTTT's text payload is "?&value1=val1&value2=val2&value3=val3"
  String IFTTTrequest = "?&value1=Hey_Man-text-POST";  // no spaces allowed 
  Serial.print ( "  text POST is     <" ); Serial.print ( IFTTTrequest ); Serial.println ( ">" ); 

#endif
  
  int IFTTTreturnCode = IFTTTpost.POST ( IFTTTrequest );     // POST the request to IFTTT

  Serial.print ( " elapsed time in ms = " ); Serial.println ( millis() - elapsed ); 

  Serial.print ( "  return code: " ); Serial.println ( IFTTTreturnCode ); 

  if ( IFTTTreturnCode > 0 ) //Check the returning code (200 is AOK)
  {
    String payload = IFTTTpost.getString();   //Get the request response payload
    Serial.println ( "  response string: <" + payload + ">" );  
  }
  
  IFTTTpost.end();   //Close HTTP connection
  
} // end postToIFTTT



