///Fonctions de connexion à NextCloud
////////

//http://cloud.debrouillonet.org/index.php/apps/sensorlogger/api/v1/createlog/

void curl_Nextcloud(String name, String key,String device,String value_name,String value){
  Serial.println("Envoi à NextCloud : "+device);
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    HTTPClient http;
    http.begin("http://cloud.debrouillonet.org/index.php/apps/sensorlogger/api/v1/createlog/");
    http.setAuthorization("j.rat","kx9ni-kDTxH-KBKPD-R4dNW-F3mK3");
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST( "\"{\"date\":\"2017-03-03 12:30:47\",  \"deviceId\":\""+device+"\",  \""+value_name+"\":\""+value+"\"}");
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);                     //Print the response payload
    }
    http.end();
  }

}
/*
{
  "date":"2017-03-03 12:30:47",
  "humidity":34.700000762939,
  "deviceId":"f7645058-fe8c-11e6-bc64-92361f002671",
  "temperature":24
}
*/
