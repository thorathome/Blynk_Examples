// ESP32 only for now
#include <HTTPClient.h>
#define HTTP_CODE_OK 200  // Good HTTP return code

// https://docs.blynk.io/en/blynk.cloud/get-datastream-value
float getBlynkDatastreamInfo ( const String& BlynkAuthcode, int datastream ) 
{
  // String BlynkAuthcode = Blynk authcode of device we are getting data FROM
  // int datastream = Blynk Datastream we are getting data FROM
  // example float value = getBlynkDatastreamInfo ( "authcode", V6 )
  //         if ( value == -123.45 ) Serial.println ( "Error" );  
  
  float returnedValue = -123.45; // default error value

  Serial.print ( "\ngetBlynkDatastreamInfo called on datastream V" ); Serial.print ( datastream ); 
  Serial.print (  " on device with authcode " ); Serial.println ( BlynkAuthcode ); 

  // Set up an HTTP client object for the Blynk get 
  HTTPClient getBlynkHTTPclient;  
  
  // Server name URL
  String BlynkGetServerName = "https://blynk.cloud/external/api/get";
  //String BlynkGetServerName = "https://ny3.blynk.cloud/external/api/get"; // Didn't find I needed this

  // Blynk's GET payload for getting data is "?token={token}&V8"
  String BlynkRequest = "?&token=" + String ( BlynkAuthcode ) + "&v" + String ( datastream ); 

  // Blynk datastream get uses GET and a single request string
  String fullRequest = BlynkGetServerName + BlynkRequest; 
  
  Serial.print ( " BlynkGetServerName <" ); Serial.print ( BlynkGetServerName ); Serial.println ( ">" ); 
  Serial.print ( "  BlynkRequest     <" ); Serial.print ( BlynkRequest ); Serial.println ( ">" ); 
  Serial.print ( "  fullRequest is   <" ); Serial.print ( fullRequest ); Serial.println ( ">" ); 

  getBlynkHTTPclient.begin ( fullRequest );  //Specify URL AND request

  //getBlynkHTTPclient.addHeader ( "Content-Type", "text/plain" );
  //getBlynkHTTPclient.addHeader ( "Content-Type", "application/x-www-form-urlencoded" );
  
  long elapsed = millis();  
  
  int BlynkReturnCode = getBlynkHTTPclient.GET();     // POST the request to IFTTT
  Serial.print ( " elapsed time in ms = " ); Serial.println ( millis() - elapsed ); 

  Serial.print ( "  return code: " ); Serial.println ( BlynkReturnCode ); 

  if ( BlynkReturnCode > 0 ) //Check the returning code (200 is AOK)
  {
    String payload = getBlynkHTTPclient.getString();   //Get the request response payload
    Serial.println ( "  response string: <" + payload + ">" );  

    if ( BlynkReturnCode == HTTP_CODE_OK )
    {
      returnedValue = payload.toFloat();   
      Serial.print ( "  Returning value: " ); Serial.println ( returnedValue ); Serial.println();  
    }
  }
  
  getBlynkHTTPclient.end();   //Close HTTP connection

  return returnedValue;
  
} // end getBlynkDatastreamInfo





// https://docs.blynk.io/en/blynk.cloud/update-datastream-value
bool updateBlynkDatastreamInfo ( const String& BlynkAuthcode, int datastream, float value ) 
{
  // String BlynkAuthcode = Blynk authcode of device we are writing TO
  // int datastream = Blynk Datastream we are writing TO
  // float value = value to write into external datastream
  // example if ( ! updateBlynkDatastreamInfo ( "authcode", V6, 267.8 ) ) Serial.println ( "Error" );

  bool returnCode; // = false on fail/true on success
  
  Serial.print ( "\nupdateBlynkDatastreamInfo called on datastream V" ); Serial.print ( datastream ); 
  Serial.print (  " on device with authcode " ); Serial.println ( BlynkAuthcode ); 
  Serial.print (  " with data " ); Serial.println ( value ); 
  
  // Set up an HTTP client object for the Blynk update 
  HTTPClient updateBlynkHTTPclient;  
  
  // Server name URL
  String BlynkUpdateServerName = "https://blynk.cloud/external/api/update";
  //String BlynkUpdateServerName = "https://ny3.blynk.cloud/external/api/update";  // Didn't find I needed this

  // Blynk's GET payload for an update is "?token={token}&dataStreamId={id}&value={value}"
  String BlynkRequest = "?token=" + BlynkAuthcode          // Authcode of device to be updated
                       + "&v"     + String ( datastream )  // Virtual pin to update
                       + "="      + String ( value );      // value to update

  // Blynk datastream update uses GET and a single request string
  String fullRequest = BlynkUpdateServerName + BlynkRequest; 
  
  Serial.print ( " BlynkUpdateServerName <" ); Serial.print ( BlynkUpdateServerName ); Serial.println ( ">" ); 
  Serial.print ( "  BlynkRequest        <" ); Serial.print ( BlynkRequest );          Serial.println ( ">" ); 
  Serial.print ( "  fullRequest    is   <" ); Serial.print ( fullRequest);            Serial.println ( ">" ); 

  long elapsed = millis();  

  updateBlynkHTTPclient.begin ( fullRequest );  // Blynk GET wants the URL AND request

  updateBlynkHTTPclient.addHeader ( "Content-Type", "text/plain" );
  //updateBlynkHTTPclient.addHeader ( "Content-Type", "application/x-www-form-urlencoded" );
  
  int BlynkReturnCode = updateBlynkHTTPclient.GET();         // use GET to send the update request to Blynk
  Serial.print ( " elapsed time in ms = " ); Serial.println ( millis() - elapsed ); 

  Serial.print ( "  return code: " ); Serial.println ( BlynkReturnCode ); 
  if ( BlynkReturnCode > 0 ) //Check the returning code (200 is AOK)
  {
    String payload = updateBlynkHTTPclient.getString();   //Get the request response payload
    Serial.println ( "  response string: <" + payload + ">" );  
  }
  
  if ( BlynkReturnCode == HTTP_CODE_OK ) returnCode = true; else returnCode = false;
  return returnCode;  
  
} // end updateBlynkDatastreamInfo
