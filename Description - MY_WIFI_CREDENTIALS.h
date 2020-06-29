/* MY_WIFI_CREDENTIALS.h
 * 
 * Even if you use only one SSID/WiFiPassword, you may wish to 
 * keep it in a central place, out of your code. 
 * 
 * A simple way to manage SSIDs and WiFiPasswords you may wish to use...
 * * Put your WiFi credentials in a file you can #include in your Arduino sketches. 
 * * Use a simple label like MY_SSID AND MY_WIFI_PASSWORD in your code.  
 * 
 * Here's how...
 * * Create a new folder called MY_WIFI_CREDENTIALS in the Arduino/libraries folder
 * * Create a text file named MY_WIFI_CREDENTIALS.h 
 *   (Same as  as the folder. The .h is important.)
 * 
 * * Use this as a template.
 * * Edit in your own WiFi SSID(s) and WiFi Password(s)
 */ 


// #include "MY_WIFI_CREDENTIALS.h"   // #defines MY_WIFI_SSID AND MY_WIFI_PASSWORD

#define MY_WIFI_SSID "Serengeti"                        // network SSID (name)
#define MY_WIFI_PASSWORD "hey-this is secreet 9897%%&// network password

/* Comment out the credentials you do not use as regularly...
#define MY_WIFI_SSID "Virus-Vault"                        // network SSID (name)
#define MY_WIFI_PASSWORD "TwoFourSixEight2468!"  // network password

#define MY_WIFI_SSID "MyAndroidPhone"                          // network SSID (name)
#define MY_WIFI_PASSWORD "AndroidPassword66$"                // network password
*/

/*
 * * With the library file in place, restart your Arduino IDE.
 * * In each Arduino sketch, add the line 
 *   #include "MY_WIFI_CREDENTIALS.h" // Use quotes " here.
 * * In all your sketches say something like 
 * * Write each sketch to use MY_SSID and MY_WIFI_PASSWORD throughout the sketch 
 * * Compile and go
 * 
 * This way, 
 * * All your WiFi credentials are in one place where you can keep track of them.
 * * You are not hard-coding passwords in your sketches.
 * * You Arduino code becomes more portable because all sketches use MY_WIFI_CREDENTIALS.
 * * If you ever have to publish your code, your WiFi Credentials are not visible to everyone.
 * * You will be happier in life.
 */
