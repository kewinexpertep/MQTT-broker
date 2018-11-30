#include "Stream.h"

#define LOCAL                    0
#define GLOBAL                   1
class LocalMqtt {
    private:
        char* extensionStr (char* extension,char* receiver);
        char* typeTopic (char* topic,char* extension, unsigned char type);
    public:
        LocalMqtt();
        void connect(char* deviceName,char* mqtt_user,char* mqtt_password);
        bool connected();
        void loop();
        void subscribe (char* topic,unsigned char type);
        void publish (char* topic,float payload,unsigned char type);
        void publish (char* topic,int payload,unsigned char type);
        void publish (char* topic,const char* payload,unsigned char type);
        void chat (char* receiver,int payload);
        void chat (char* receiver,float payload);
        void chat (char* receiver,const char* payload);
        bool initServer (char* mqtt_server,int port);
        void setCallback(void (* callback)(char*, uint8_t*,unsigned int));
        void callback(char* topic, uint8_t* payload, unsigned int length);
};