Date 20 June 2021

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

I hope it's helpful, have also posted this code on Blynk Community. 
