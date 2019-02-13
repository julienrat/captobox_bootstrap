///Fonctions de connexion à IFTTT
////////


void curl_IFTTT(String name_trigger,String key, String value1, String value2, String value3){
  Serial.println("Envoi à IFTTT : "+name_trigger);
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    HTTPClient http;
    http.begin("http://maker.ifttt.com/trigger/"+name_trigger+"/with/key/"+key);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST( "{\"value1\":\""+value1+"\",  \"value2\":\""+value2+"\",  \"value3\":\""+value3+"\"}");
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);                     //Print the response payload
    }
    http.end();
  }
}
