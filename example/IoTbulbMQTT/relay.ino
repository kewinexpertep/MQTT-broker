#include <ESP8266WiFi.h>                  // Include ESP8266 library
#include <DHT.h>                // Include DHT sensor library
#include <LocalMqtt.h>

#define ssid "Pi-net"          // SSID is set
#define password "12345678"   // Password is set

#define D0 16
#define LED D0

#define mqtt_server "192.168.4.1"
#define mqtt_user "user"
#define mqtt_password "1234"

#define deviceName "lightEdge"
char* receiver = "light";


bool light_sw1 = false;
LocalMqtt local;
void setup_wifi() {
  Serial.print("Connecting to ");         // Print title message to the serial monitor
  Serial.println(ssid);                   // Print SSID name
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 0);                // WiFi detected indicator - active low
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);             // Send password
  while (WiFi.status() != WL_CONNECTED)   // Check WiFi status
  {
    delay(1000);
    Serial.println("connection WiFi failed");   // Print error message to Serial Monitor

  }
  Serial.println("");
  Serial.println("WiFi connected");       // Print the connected message
  Serial.println("IP address: ");         // Print IP address
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length)
{
    payload[length] = '\0';
    Serial.print((char*)topic);
    Serial.print(" : ");
    Serial.println((char*)payload);
    const char* sw = (light_sw1)?"1":"0";
    switch ((char)payload[0]) {
        case '0': light_sw1 = false;
          sw = "0";
          local.publish("lightEdge",sw,GLOBAL);
          break;
        case '1': light_sw1 = true;
          sw = "1";
          local.publish("lightEdge",sw,GLOBAL);
          break;
        case '?': local.publish("lightEdge",sw,GLOBAL);
          break;
    }
}
void setup()
{
  Serial.begin(115200);                   // Enable UART
  Serial.println();
  pinMode(LED, OUTPUT); 
  setup_wifi();
  local.initServer(mqtt_server, 1883);
  local.setCallback(callback);
}

void loop()
{
  // WiFi connected indicator - active low
  while (WiFi.status() != WL_CONNECTED)   // Check WiFi status
  {
    delay(500);
    Serial.println("connection WiFi failed");   // Print error message to Serial Monitor
  }
  while (!local.connected()) {
      local.connect(deviceName, mqtt_user, mqtt_password);
  }
  
  local.loop();
  
  uint8_t state;
  if(light_sw1){
    state = HIGH;
  }else{
    state = LOW;
  }
  digitalWrite(LED, state);
}
