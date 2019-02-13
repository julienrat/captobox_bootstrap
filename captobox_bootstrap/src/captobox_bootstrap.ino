
//////////////////////////////////
// Captobox _ Bootstrap Version //
//////////////////////////////////
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <Wire.h>
#include <BME280_MOD-1022.h>
#include <iAQ-MOD1023.h>

#include <FS.h>

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266HTTPClient.h>

#include <TimeLib.h>
#include <Ticker.h>

#include "sensors.h"

/////////////Configuration de base
String name = "Captobox";
String access_point = "Captobox";
String version = "bootstrap";
////////////////////////Variables Globales


String rate;
String saving = "false";
String ap_mode = "true";
String ssid;
String pwd;
String canal;
String host;
String feed;
String login_name;
String key;
String time_computer;
String latitude;
String longitude;

IPAddress captobox_ip;

boolean entete = true;
boolean send_to_cloud = false;
boolean create_adafruit_cloud = false;
//////////Démarrage Serveur

AsyncWebServer server(80);
//AsyncWebServer httpsserver(443);
/////////////////////////

Ticker refresh;
FSInfo fs_info;


#include "adafruit_cloud.h"
#include "IFTTT_cloud.h"
//#include "Next_cloud.h"
void setup() {
  pinMode(BUILTIN_LED,OUTPUT);
  Serial.begin(115200);
  Wire.begin(TX, RX);
//  Wire.begin();
  Wire.setClockStretchLimit(1500);

  SPIFFS.begin();
  read_config();
  ap_mode="false";  ///A virer
  setup_wifi(name, ssid, pwd, canal);
  refresh.attach_ms(rate.toInt(), refresh_values);
  serveur_html();
  server.begin();

}

void loop() {
  if(create_adafruit_cloud && host.equals("Adafruit IO")){
    curl_adafruit_dashboard("captobox", key, login_name);
    curl_adafruit_feed(name_capt[0],key,login_name);
    curl_adafruit_feed(name_capt[1],key,login_name);
    curl_adafruit_feed(name_capt[2],key,login_name);
    curl_adafruit_feed(name_capt[3],key,login_name);
    curl_adafruit_block(name_capt[0], "captobox",name_capt[0],key,login_name);
    curl_adafruit_block(name_capt[1], "captobox",name_capt[1],key,login_name);
    curl_adafruit_block(name_capt[2], "captobox",name_capt[2],key,login_name);
    curl_adafruit_block(name_capt[3], "captobox",name_capt[3],key,login_name);
    create_adafruit_cloud=false;
  }

  if (send_to_cloud && host.equals("Adafruit IO")){
   curl_adafruit_values(value_capt[0],name_capt[0],key,login_name);
   curl_adafruit_values(value_capt[1],name_capt[1],key,login_name);
   curl_adafruit_values(value_capt[2],name_capt[2],key,login_name);
   curl_adafruit_values(value_capt[3],name_capt[3],key,login_name);
  send_to_cloud=false;
  }

  if (send_to_cloud && host.equals("IFTTT")){
    Serial.println("send to IFTTT...");
    curl_IFTTT(login_name,key,name_capt[0]+" : "+value_capt[0],name_capt[1]+" : "+value_capt[1], name_capt[2]+" : "+value_capt[2]);
    send_to_cloud=false;
  }
}

void setup_wifi(String name, String ssid, String mdp, String canal) {
  int timeout=0;
  WiFi.hostname(name.c_str());
  WiFi.begin(ssid.c_str(), mdp.c_str());
  Serial.println("connection en cours...");
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(BUILTIN_LED,LOW);
    delay(180);
    timeout++;
    Serial.print(".");
    digitalWrite(BUILTIN_LED,HIGH);
    delay(80);
    if (timeout > 60) { // On reboucle en point d'acces si la connexion a échouée
      Serial.println("TimeOUT");
      digitalWrite(BUILTIN_LED,LOW);
      return;
    }
  }
WiFi.softAP(name.c_str(), "", 10);
captobox_ip = WiFi.localIP();
Serial.println("");
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(captobox_ip);

  //MDNS.addService("http", "tcp", 80);
  //MDNS.begin(name.c_str());

}
void extract_sensor() {
  for (int i = 0; i < nb_of_sensors; i++) {
    if (name_capt[i].equals(sensors_labels[i])) {
      Serial.print("hello");
    }
  }
}

void refresh_values() {
  digitalWrite(BUILTIN_LED,LOW);
  for (int i = 0; i < nb_of_sensors; i++) {
    for (int j = 0; j < nb_of_sensors; j++) {
      if (name_capt[i].equals(sensors_labels[j])) {
         mesure(j, i);
      }
    }
    digitalWrite(BUILTIN_LED,HIGH);
  }
  write_config( name_capt[0], value_capt[0],  value_min_capt[0],  value_max_capt[0],  value_unit_capt[0],
                name_capt[1], value_capt[1],  value_min_capt[1],  value_max_capt[1],  value_unit_capt[1],
                name_capt[2], value_capt[2],  value_min_capt[2],  value_max_capt[2],  value_unit_capt[2],
                name_capt[3], value_capt[3],  value_min_capt[3],  value_max_capt[3],  value_unit_capt[3],
                name_capt[4], value_capt[4],  value_min_capt[4],  value_max_capt[4],  value_unit_capt[4],
                rate,  saving,  ap_mode,  ssid,  pwd,  canal,  host, feed,  login_name,  key);

  if (saving == "true" && disk_full() == false) {
    write_data(name_capt[0], name_capt[1], name_capt[2], name_capt[3], value_capt[0], value_capt[1], value_capt[2], value_capt[3]);
  }
  send_to_cloud=true;
}


void serveur_html() { // Ici toutes les redirection vers les pages Web

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->redirect("/index.html");
    //delay(100);
  });

  server.onNotFound([](AsyncWebServerRequest * request) {
    request->redirect("/index.html");
    //delay(100);
  });

  //server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.serveStatic("/", SPIFFS, "/", "max-age=86400");
  /////////////requetes capteurs
  server.on("/save", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (saving.equals("true")) {
      saving = "false";
    } else {
      saving = "true";
    }
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", saving);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/config", HTTP_GET, [](AsyncWebServerRequest * request) {
    // request->redirect("/config.json");
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/config.json" );
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/data.csv", "application/octet-stream" );
    response->addHeader("Access-Control-Allow-Origin", "*");
    //response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/test", HTTP_GET, [](AsyncWebServerRequest * request) {
    int test_rand = random(0, 1024);
    Serial.println("Requete test : " + String(test_rand));
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", String(test_rand) );
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/sensor_value", HTTP_GET, [](AsyncWebServerRequest * request) {
    String id = request->arg("num");
    String sensor_value = value_capt[id.toInt()-1];
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", sensor_value);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/sensor_name", HTTP_GET, [](AsyncWebServerRequest * request) {
    String id = request->arg("num");
    String sensor_name = name_capt[id.toInt()-1];
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", sensor_name);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/set_position", HTTP_GET, [](AsyncWebServerRequest * request) {
    latitude = request->arg("lat");
    longitude = request->arg("lon");
    Serial.println("Position :" +latitude+" : "+longitude);
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "position ok Lat : "+latitude+" Long : "+ longitude);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/set_time", HTTP_GET, [](AsyncWebServerRequest * request) { //Mise à l'heure de la Captobox
    String datetime_h = request->arg("datetime_h");
    String datetime_m = request->arg("datetime_m");
    String datetime_s = request->arg("datetime_s");
    String datetime_d = request->arg("datetime_d");
    String datetime_M = request->arg("datetime_M");
    String datetime_y = request->arg("datetime_y");

    setTime(datetime_h.toInt(), datetime_m.toInt(), datetime_s.toInt(), datetime_d.toInt(), datetime_M.toInt(), datetime_y.toInt());
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Mise à l'heure OK" );
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/get_space", HTTP_GET, [](AsyncWebServerRequest * request) {
    SPIFFS.info(fs_info);
    float free_mem = (fs_info.totalBytes - fs_info.usedBytes) / (1000000.0);
    float total_mem = ((fs_info.totalBytes - 0.5) / 1000000.0);
    int32_t estimated_time_s = ((free_mem - 0.5) * 1000000.0) / 30.0 * rate.toInt() / 1000.0;
    int h_time, m_time, s_time;
    uint32_t t = estimated_time_s;
    s_time = t % 60;
    t = (t - s_time) / 60;
    m_time = t % 60;
    t = (t - m_time) / 60;
    h_time = t;

    String reponse = "Mémoire disponible : "
                     + String(free_mem)
                     + " Mo" + " sur " + String(total_mem) + " Mo <br>"
                     + "Temps d'enregistrement estimé : "
                     + String(h_time) + "h " + String(m_time) + "m " + String(s_time) + "s\n";



    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", reponse );
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });
  server.on("/get_ip", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain",ssid+"  IP : "+ captobox_ip.toString());
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });
  server.on("/erase_file", HTTP_GET, [](AsyncWebServerRequest * request) {
    saving = "false";
    erase_file(name_capt[0], name_capt[1], name_capt[2], name_capt[3]);
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Fichier effacé" );
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/write", HTTP_GET, [](AsyncWebServerRequest * request) {
    /*ecriture du ficher d'echange
      /write?value_capt_1=0&value_min_capt_1=0&value_max_capt_1=0&value_unit_capt_1&value_capt_2=0&value_min_capt_2=0&value_max_capt_2=0&value_unit_capt_2&value_capt_3=0&value_min_capt_3=0&value_max_capt_3=0&value_unit_capt_3&value_capt_4=0&value_min_capt_4=0&value_max_capt_4=0&value_unit_capt_4&value_capt_5=0&value_min_capt_5=0&value_max_capt_5=0&value_unit_capt_5&rate=0&saving=0&ap_mode=0&ssid=0&pwd=0&canal=0&host=0&login_name=0&key=123
    */

    for (int i = 0; i < 4; i++) {
      name_capt[i] = request->arg("name_capt_" + String(i + 1));
      value_capt[i] = request->arg("value_capt_" + String(i + 1));
      value_min_capt[i] = request->arg("value_min_capt_" + String(i + 1));
      value_max_capt[i] = request->arg("value_max_capt_" + String(i + 1));
      value_unit_capt[i] = request->arg("value_unit_capt_" + String(i + 1));
      Serial.println(name_capt[i]);
    }

    saving = request->arg("saving");
    ap_mode = request->arg("ap_mode");
    ssid = request->arg("ssid");
    pwd = request->arg("pwd");

    rate = request->arg("rate");

    canal = request->arg("canal");
    host = request->arg("host");
    feed = request->arg("feed");
    login_name = request->arg("login_name");
    key = request->arg("key");

    write_config( name_capt[0], value_capt[0],  value_min_capt[0],  value_max_capt[0],  value_unit_capt[0],
                  name_capt[1], value_capt[1],  value_min_capt[1],  value_max_capt[1],  value_unit_capt[1],
                  name_capt[2], value_capt[2],  value_min_capt[2],  value_max_capt[2],  value_unit_capt[2],
                  name_capt[3], value_capt[3],  value_min_capt[3],  value_max_capt[3],  value_unit_capt[3],
                  name_capt[4], value_capt[4],  value_min_capt[4],  value_max_capt[4],  value_unit_capt[4],
                  rate,  saving,  ap_mode,  ssid,  pwd,  canal,  host, feed,  login_name,  key);
    Serial.println("Write measure");
    create_adafruit_cloud=true;
    Serial.println("Reset..");

    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Configuration Enregistrée" );
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);

    //delay(10000);


  });

}

boolean disk_full() {
  SPIFFS.info(fs_info);
  float used_mem = (fs_info.usedBytes) / (1000000.0);
  float total_mem = (fs_info.totalBytes / 1000000.0);
  Serial.print("taille max : ");
  Serial.println(total_mem - 1);
  Serial.print("taille usée : ");
  Serial.println(used_mem);
  if (used_mem > (total_mem - 1)) {
    return true;
  } else {
    return false;
  }
}



void write_config(String name_capt_1, String value_capt_1, String value_min_capt_1, String value_max_capt_1, String value_unit_capt_1,
                  String name_capt_2, String value_capt_2, String value_min_capt_2, String value_max_capt_2, String value_unit_capt_2,
                  String name_capt_3, String value_capt_3, String value_min_capt_3, String value_max_capt_3, String value_unit_capt_3,
                  String name_capt_4, String value_capt_4, String value_min_capt_4, String value_max_capt_4, String value_unit_capt_4,
                  String name_capt_5, String value_capt_5, String value_min_capt_5, String value_max_capt_5, String value_unit_capt_5,
                  String rate, String saving, String ap_mode, String ssid, String pwd, String canal, String host, String feed, String login_name, String key) {
  const size_t bufferSize = 6 * JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(11);
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.createObject();

  JsonObject& sensors = root.createNestedObject("sensors");

  JsonObject& sensors_capt_1 = sensors.createNestedObject("capt_1");
  sensors_capt_1["Name"] = name_capt_1;
  sensors_capt_1["Value"] = value_capt_1;
  sensors_capt_1["Min"] = value_min_capt_1;
  sensors_capt_1["Max"] = value_max_capt_1;
  sensors_capt_1["Unit"] = value_unit_capt_1;

  JsonObject& sensors_capt_2 = sensors.createNestedObject("capt_2");
  sensors_capt_2["Name"] = name_capt_2;
  sensors_capt_2["Value"] = value_capt_2;
  sensors_capt_2["Min"] = value_min_capt_2;
  sensors_capt_2["Max"] = value_max_capt_2;
  sensors_capt_2["Unit"] = value_unit_capt_2;

  JsonObject& sensors_capt_3 = sensors.createNestedObject("capt_3");
  sensors_capt_3["Name"] = name_capt_3;
  sensors_capt_3["Value"] = value_capt_3;
  sensors_capt_3["Min"] = value_min_capt_3;
  sensors_capt_3["Max"] = value_max_capt_3;
  sensors_capt_3["Unit"] = value_unit_capt_3;

  JsonObject& sensors_capt_4 = sensors.createNestedObject("capt_4");
  sensors_capt_4["Name"] = name_capt_4;
  sensors_capt_4["Value"] = value_capt_4;
  sensors_capt_4["Min"] = value_min_capt_4;
  sensors_capt_4["Max"] = value_max_capt_4;
  sensors_capt_4["Unit"] = value_unit_capt_4;

  JsonObject& sensors_capt_5 = sensors.createNestedObject("capt_5");
  sensors_capt_5["Name"] = name_capt_5;
  sensors_capt_5["Value"] = value_capt_5;
  sensors_capt_5["Min"] = value_min_capt_5;
  sensors_capt_5["Max"] = value_max_capt_5;
  sensors_capt_5["Unit"] = value_unit_capt_5;

  root["Refresh_rate"] = rate;
  root["Saving"] = saving;
  root["Is_ap"] = ap_mode;
  root["Ssid"] = ssid;
  root["Password"] = pwd;
  root["Channel"] = canal;
  root["Host"] = host;
  root["Feed"] = feed;
  root["Login"] = login_name;
  root["Streaming_Key"] = key;
  File configuration = SPIFFS.open("/config.json", "w");
  root.printTo(configuration);
  configuration.close();
}

void erase_file(String name_capt0, String name_capt1, String name_capt2, String name_capt3) {
  File data;
  data = SPIFFS.open("/data.csv", "w");
  data.println("Date," + name_capt0 + "," + name_capt1 + "," + name_capt2 + "," + name_capt3+",Lat,Lon");
  data.close();
}

void write_data(String name_capt0, String name_capt1, String name_capt2, String name_capt3, String value_capt0, String value_capt1, String value_capt2, String value_capt3) {
  File data;
  data = SPIFFS.open("/data.csv", "a");
  data.println(String(hour()) + ":" + String(minute()) + ":" + String(second()) + "," + value_capt0 + "," + value_capt1 + "," + value_capt2 + "," + value_capt3+","+latitude+","+longitude);
  data.close();
}

void read_config() {
  const size_t bufferSize = 6 * JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(11) + 570;
  DynamicJsonBuffer jsonBuffer(bufferSize);
  File configuration = SPIFFS.open("/config.json", "r");
  JsonObject& root = jsonBuffer.parseObject(configuration);

  JsonObject& sensors = root["sensors"];

  JsonObject& sensors_capt_1 = sensors["capt_1"];
  const char* sensors_capt_1_Name = sensors_capt_1["Name"]; // "name"
  const char* sensors_capt_1_Value = sensors_capt_1["Value"]; // "value"
  const char* sensors_capt_1_Min = sensors_capt_1["Min"]; // "value_min"
  const char* sensors_capt_1_Max = sensors_capt_1["Max"]; // "value_max"
  const char* sensors_capt_1_Unit = sensors_capt_1["Unit"]; // "value_unit"

  JsonObject& sensors_capt_2 = sensors["capt_2"];
  const char* sensors_capt_2_Name = sensors_capt_2["Name"]; // "name"
  const char* sensors_capt_2_Value = sensors_capt_2["Value"]; // "value"
  const char* sensors_capt_2_Min = sensors_capt_2["Min"]; // "value_min"
  const char* sensors_capt_2_Max = sensors_capt_2["Max"]; // "value_max"
  const char* sensors_capt_2_Unit = sensors_capt_2["Unit"]; // "value_unit"

  JsonObject& sensors_capt_3 = sensors["capt_3"];
  const char* sensors_capt_3_Name = sensors_capt_3["Name"]; // "name"
  const char* sensors_capt_3_Value = sensors_capt_3["Value"]; // "value"
  const char* sensors_capt_3_Min = sensors_capt_3["Min"]; // "value_min"
  const char* sensors_capt_3_Max = sensors_capt_3["Max"]; // "value_max"
  const char* sensors_capt_3_Unit = sensors_capt_3["Unit"]; // "value_unit"

  JsonObject& sensors_capt_4 = sensors["capt_4"];
  const char* sensors_capt_4_Name = sensors_capt_4["Name"]; // "name"
  const char* sensors_capt_4_Value = sensors_capt_4["Value"]; // "value"
  const char* sensors_capt_4_Min = sensors_capt_4["Min"]; // "value_min"
  const char* sensors_capt_4_Max = sensors_capt_4["Max"]; // "value_max"
  const char* sensors_capt_4_Unit = sensors_capt_4["Unit"]; // "value_unit"

  JsonObject& sensors_capt_5 = sensors["capt_5"];
  const char* sensors_capt_5_Name = sensors_capt_5["Name"]; // "name"
  const char* sensors_capt_5_Value = sensors_capt_5["Value"]; // "value"
  const char* sensors_capt_5_Min = sensors_capt_5["Min"]; // "value_min"
  const char* sensors_capt_5_Max = sensors_capt_5["Max"]; // "value_max"
  const char* sensors_capt_5_Unit = sensors_capt_5["Unit"]; // "value_unit"

  const char* Refresh_rate = root["Refresh_rate"]; // "Rate"
  const char* Saving = root["Saving"]; // "yes"
  const char* Is_ap = root["Is_ap"]; // "ap_mode"
  const char* Ssid = root["Ssid"]; // "ssid_name"
  const char* Password = root["Password"]; // "pwd"
  const char* Channel = root["Channel"]; // "canal"
  const char* Host = root["Host"]; // "host"
  const char* Feed = root["Feed"]; // "feed"
  const char* Login = root["Login"]; // "login_name"
  const char* Streaming_Key = root["Streaming_Key"]; // "key"

  name_capt[0] = String(sensors_capt_1_Name );
  value_capt[0] = String(sensors_capt_1_Value );
  value_min_capt[0] = String(sensors_capt_1_Min);
  value_max_capt[0] = String(sensors_capt_1_Max );
  value_unit_capt[0] = String(sensors_capt_1_Unit );

  name_capt[1] = String(sensors_capt_2_Name );
  value_capt[1] = String(sensors_capt_2_Value );
  value_min_capt[1] = String(sensors_capt_2_Min);
  value_max_capt[1] = String(sensors_capt_2_Max );
  value_unit_capt[1] = String(sensors_capt_2_Unit );

  name_capt[2] = String(sensors_capt_3_Name );
  value_capt[2] = String(sensors_capt_3_Value );
  value_min_capt[2] = String(sensors_capt_3_Min);
  value_max_capt[2] = String(sensors_capt_3_Max );
  value_unit_capt[2] = String(sensors_capt_3_Unit );

  name_capt[3] = String(sensors_capt_4_Name );
  value_capt[3] = String(sensors_capt_4_Value );
  value_min_capt[3] = String(sensors_capt_4_Min);
  value_max_capt[3] = String(sensors_capt_4_Max );
  value_unit_capt[3] = String(sensors_capt_4_Unit );

  name_capt[4] = String(sensors_capt_5_Name );
  value_capt[4] = String(sensors_capt_5_Value );
  value_min_capt[4] = String(sensors_capt_5_Min);
  value_max_capt[4] = String(sensors_capt_5_Max );
  value_unit_capt[4] = String(sensors_capt_5_Unit );

  rate = String( Refresh_rate);
  saving = String(Saving );
  ap_mode = String( Is_ap);
  Serial.print("ap mode : ");
  Serial.println(ap_mode);
  ssid = String(Ssid );
  pwd = String(Password );
  canal = String( Channel);
  host = String( Host);
  feed = String(Feed);
  login_name = String( Login);
  key = String(Streaming_Key);
  configuration.close();
}
