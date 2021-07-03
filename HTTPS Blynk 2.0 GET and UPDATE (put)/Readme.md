3 July 2021

Blynkers. 

I’m moving my old Blynk sketches and devices to Blynk 2.0. The **loss of the Bridge and Webhook widgets** slowed me down, initially. 

**Thanks** to @PeteKnight for the nudge. **Thanks** to @Blynk_Coeur for a code snippet that got me going.  

Helpful Blynk doc for both [GET](https://docs.blynk.io/en/blynk.cloud/get-datastream-value) and [UPDATE](https://docs.blynk.io/en/blynk.cloud/update-datastream-value) are also cited in the sketches. 

Two routines follow, both written for ESP32. (ESP8266 mods are small, but I didn’t want to complicate.)
•	getBlynkDatastreamInfo - get data from another datastream (read from)
•	updateBlynkDatastreamInfo - update data in another datastream (write to)

**getBlynkDatastreamInfo**
This sketch gets or pulls the current value from a remote Blynk datastream on another device. The sketch, below, is written as a `float` function, delivering a `float` value. It returns a value of -123.45 (or whatever you set it as) on an error. 

**updateBlynkDatastreamInfo**
This sketch updates or pushes a value to a remote Blynk datastream on another device. The sketch is written as a Boolean function, returning true or false on success.  

**For both routines...**
Parameters
-	BlynkAuthcode of the remote device you are getting data from or updating data to.
-	Datastream number (Virtual pin) you are getting data from or updating data to
-	Value ( updateBlynkDatastreamInfo only ) is the value to update the datastream with

#includes
-	On ESP32, all we need is the usual Blynk #includes PLUS `#include <HTTPClient.h>`

#defines
-	`#define HTTP_CODE_OK 200  // Good HTTP return code for code clarity`

Timing
-	I timed the HTTP.GETs using `millis()` as in @PeteKnight 's sketch, above. 
Mine came in at 900-1000 ms, mixing gets and updates to and from multiple devices using this code. 
(I get very similar timing when POSTing to IFTTT using my previously posted code.)

**Comments**
This code over-uses `String`s for clarity. Feel free to simplify. 
This code over-uses `Serial.print`s, as well. Helps me keep sane. 
I use the general blynk.cloud domain without issues. 
I tried both Content types for the HTTP GETs: "text/plain", 
and "application/x-www-form-urlencoded". Both work. No noted change in elapsed runtimes.  

I have also posted these sketches in Blynk Community
[https://community.blynk.cc/t/webhook-widget/54152/30](https://community.blynk.cc/t/webhook-widget/54152/30)

Hope this helps the cause. 
**All comments welcome.**  
