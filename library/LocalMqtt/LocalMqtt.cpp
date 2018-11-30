#include "LocalMqtt.h"
#include "PubSubClient.h"
#include <ESP8266WiFi.h>// Include ESP8266 library
void (* cb_message)(char*, uint8_t*,unsigned int);
WiFiClient espClient;
PubSubClient client(espClient);

LocalMqtt::LocalMqtt () {
    
}
void LocalMqtt::connect(char* deviceName,char* mqtt_user,char* mqtt_password) {
    if (client.connect(deviceName, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      char* newTopic = extensionStr("gearLocal/",deviceName);
      client.subscribe(newTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("Please check your Username Password and DeviceName");
      // Wait 5 seconds before retrying
      delay(5000);
    }
}
bool LocalMqtt::connected() {
  return client.connected();
}
void LocalMqtt::loop() {
  client.loop();
}
void LocalMqtt::subscribe (char* topic,unsigned char type) {
  char* newTopic = typeTopic (topic,"netpie/", type);
  client.subscribe(newTopic);
}
char* LocalMqtt::typeTopic (char* topic,char* extension, unsigned char type) {
  char* newTopic;
  switch (type) {
    case LOCAL:newTopic = topic;break; //local
    case GLOBAL:newTopic = extensionStr(extension,topic);break;  //Global
  }
  return newTopic;
}
void LocalMqtt::publish (char* topic,float payload,unsigned char type) {
  char* newTopic = typeTopic (topic,"global/", type);
  client.publish(newTopic, String(payload).c_str());
}
void LocalMqtt::publish (char* topic,int payload,unsigned char type) {
  char* newTopic = typeTopic (topic,"global/", type);
  client.publish(newTopic, String(payload).c_str());
}
void LocalMqtt::publish (char* topic,const char* payload,unsigned char type) {
  char* newTopic = typeTopic (topic,"global/", type);
  client.publish(newTopic, payload);
}
void LocalMqtt::chat (char* receiver,int payload) {
  char* extension = "gearLocal/";
  char* newTopic = extensionStr(extension,receiver);
  client.publish(newTopic, String(payload).c_str());
}
void LocalMqtt::chat (char* receiver,float payload) {
  char* extension = "gearLocal/";
  char* newTopic = extensionStr(extension,receiver);
  client.publish(newTopic, String(payload).c_str());
}
void LocalMqtt::chat (char* receiver,const char* payload) {
  char* extension = "gearLocal/";
  char* newTopic = extensionStr(extension,receiver);
  client.publish(newTopic, payload);
}
char* LocalMqtt::extensionStr (char* extension,char* receiver) {
  char* newTopic;
  newTopic = (char*)malloc(strlen(receiver)+1+strlen(extension)); /* make space for the new string (should check the return value ...) */
  strcpy(newTopic, extension); /* copy name into the new var */
  strcat(newTopic, receiver); /* add the extension */
  return (char*)newTopic;
}
bool LocalMqtt::initServer (char* mqtt_server,int port) {
  client.setServer(mqtt_server, port);
}
void LocalMqtt::setCallback(void (* callbackFuncName)(char*, uint8_t*,unsigned int)) {
  cb_message = callbackFuncName;
  client.setCallback(callbackFuncName);
}
void LocalMqtt::callback(char* topic, uint8_t* payload, unsigned int length) {
  cb_message(topic,payload,length);
}