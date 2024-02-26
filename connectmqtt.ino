#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Jsnr1999-2.4G";
const char* pass = "9963066008";

const char* ID = "ESP32Publish";      //name of the device
const char* TOPIC = "mfrc522/tagid";  //Topic to subscribe

const char* broker = "192.168.1.23";
WiFiClient espclient;
PubSubClient client(espclient);

void wifi_setup(){
  Serial.print(F("Connecting to "));Serial.println(ssid);
  WiFi.begin(ssid,pass);
  while(WiFi.status() != WL_CONNECTED){delay(1000);Serial.print(F("."));}
  Serial.println();
  Serial.print(F("wifi connected to "));Serial.println(ssid);
  Serial.print(F("Connected to IP address "));Serial.println(WiFi.localIP());
}

void mqtt_reconnect(){
  while(!client.connected()){
    Serial.println(F("Trying to connect to MQTT again..."));
    if(client.connect(ID)){
      Serial.println(F("connected"));
      Serial.print(F("publishing to "));Serial.println(TOPIC);
      delay(500);
    }
  }
}

void setup(){
  Serial.begin(115200);
  wifi_setup();
  client.setServer(broker, 1883);
}

void loop(){
  if(!client.connected()){mqtt_reconnect();}
  client.loop();
  client.publish(TOPIC, "Hello");
}