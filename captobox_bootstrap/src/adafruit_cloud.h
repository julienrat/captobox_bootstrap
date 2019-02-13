///Fonctions de connexion à ADAFRUIT.IO
///Générateur de requetes :
///http://editor.swagger.io/
///https://github.com/adafruit/Adafruit_IO_Arduino
/////////////////////////////////////////////////////////
String format_string(String input_string){
  input_string.toLowerCase();
  String string_formated = input_string;
  string_formated.replace("è","e");
  string_formated.replace("é","e");
  string_formated.replace("ç","c");
  string_formated.replace(" ","_");
  Serial.println("chaine formatée : "+string_formated);
  return string_formated;
}

void curl_adafruit_values(String value,String name_feed,String key,String user){

  Serial.println("creation envoi des valeurs: "+name_feed);
  HTTPClient http;
//  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    http.begin("http://io.adafruit.com/api/v2/"+format_string(user)+"/feeds/"+format_string(name_feed)+"/data");
    http.addHeader("X-AIO-Key", key);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST( "{\"value\": \""+value+"\",  \"created_at \": \""+String(day())+"-"+String(month())+ "-"+String(year()) +" " +String(hour()) + ":" + String(minute()) + ":" + String(second()) +  "\",\"lat\": \""+latitude+"\",  \"lon\": \""+longitude+"\"}");
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);                     //Print the response payload
    }
    http.end();
//  }
}


/*
tion/json" -d "{  \"value\": \"12.33\",  \"created_at\": \"01-Fev-2019 13:31:37\",  \"lat\": \"string\",  \"lon\": \"string\",  \"ele\": \"string\",  \"epoch\": 0}"
*/
void curl_adafruit_feed(String name_feed,String key,String user){

  Serial.println("creation du feed : "+name_feed);
  HTTPClient http;
//  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    http.begin("http://io.adafruit.com/api/v2/"+format_string(user)+"/feeds");
    http.addHeader("X-AIO-Key", key);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST( "{\"name\": \""+format_string(name_feed)+"\",  \"key\": \""+format_string(name_feed)+"\",  \"description\": \""+name_feed+"\",  \"license\": \"CCbySA\"}");
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);                     //Print the response payload
    }
    http.end();
//  }
}
//HTTPClient http;
void curl_adafruit_block(String name_block, String name_dashboard,String name_feed,String key,String user){
Serial.println("creation du block : "+name_block);

//  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
   HTTPClient http;

    http.begin("http://io.adafruit.com/api/v2/"+format_string(user)+"/dashboards/"+format_string(name_dashboard)+"/blocks");
    http.addHeader("X-AIO-Key", key);
    http.addHeader("Content-Type", "application/json");
    http.GET();
    String payload_test = http.getString();
    Serial.println(payload_test.indexOf(name_block));
    http.end();
    if(payload_test.indexOf(name_block)<0){
      //Declare an object of class HTTPClient
      http.begin("http://io.adafruit.com/api/v2/"+format_string(user)+"/dashboards/"+format_string(name_dashboard)+"/blocks");  //Specify request destination
      http.addHeader("X-AIO-Key", key);
      http.addHeader("Content-Type", "application/json");
      Serial.println("{\"id\": \"1234\",  \"name\": \""+format_string(name_block)+"\",  \"description\": \"string\",  \"key\": \"123\",\"dashboard_id\": \""+name_dashboard+"\",  \"visual_type\": \"line_chart\",  \"column\": 100,\"row\": 1,  \"size_x\": 0,  \"size_y\": 0,    \"block_feeds\": [    {\"feed_id\": \""+format_string(name_feed)+"\",      \"group_id\": \"string\"    }  ],\"properties\": {  \"historyHours\":1}}");

      int httpCode = http.POST("{\"id\": \"1234\",  \"name\": \""+name_block+"\",  \"description\": \"string\",  \"key\": \"123\",\"dashboard_id\": \""+name_dashboard+"\",  \"visual_type\": \"line_chart\",  \"column\": 1,\"row\": 1,  \"size_x\": 10,  \"size_y\": 5,    \"block_feeds\": [    {\"feed_id\": \""+format_string(name_feed)+"\",      \"group_id\": \""+format_string(user)+"\"    }  ],\"properties\": {  \"historyHours\":1}}");
      Serial.println("requete ADAFRUIT");
      Serial.println(httpCode);
      if (httpCode > 0) { //Check the returning code
        String payload = http.getString();   //Get the request response payload
        Serial.println(payload);                     //Print the response payload
      }
      http.end();   //Close connection
    }
//  }
}

void curl_adafruit_dashboard(String name_dashboard,String key,String user){

  Serial.println("creation du dashboard : "+name_dashboard);
  //if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    HTTPClient http;
    http.begin("http://io.adafruit.com/api/v2/"+format_string(user)+"/dashboards");
    http.addHeader("X-AIO-Key", key);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST( "{\"name\": \""+name_dashboard+"\",  \"description\": \""+name_dashboard+"\",  \"key\": \""+name_dashboard+"\"}");
    if (httpCode > 0) { //Check the returning code
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);                     //Print the response payload
    }
    http.end();
//  }

}
