#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306();

int count = 0;
int parkingOneFree = 1;
int parkingTwoFree = 1;
int parkingIndex;
int toArduinoPin = 15;
String parkingData;
String parkingDataType;
String payloadReceived;
const char* ssid     = "RedRover";
const char* password = "";

//const char* host = "firefly03.cs.cornell.edu";
//uint16_t    host_port = 30000;

// MQTT - Artik Cloud Server params
char mqttCloudServer[]     = "api.artik.cloud"; // Server
int  mqttCloudPort         = 8883; // MQTT Port
char mqttCloudClientName[] = "Parking Sensor"; // Any Name
char mqttCloudUsername[]   = "7d6e199242fd43cdb8722f579228b5e6"; // DEVICE ID
char mqttCloudPassword[]   = "1142cca8239146d090e9f2d29aa2cd20"; //  DEVICE TOKEN
char mqttCloudTopic[]      = "/v1.1/messages/7d6e199242fd43cdb8722f579228b5e6"; // (/v1.1/messages/"DEVICE ID")

char buf[200]; // Json Data to Artik Cloud

WiFiClientSecure ipCloudStack;
//MQTTClient mqttCloudClient;
//PubSubClient mqttClient(ipCloudStack);
WiFiClientSecure SSL_ESP; // ESP SSL Connection to Artik cloud

WiFiClientSecure espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int len) {
  payloadReceived = "";
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message: ");
  for (int i = 0; i < len; i++) {
    Serial.print((char)payload[i]);
    payloadReceived = payloadReceived + (char)payload[i];
  }
  Serial.println();
  Serial.println("------------------------------");
  parkingIndex = payloadReceived.indexOf("parking");
  parkingDataType = payloadReceived.substring(parkingIndex, parkingIndex + 14);
  parkingData = payloadReceived.substring(parkingIndex + 17, parkingIndex + 18);
  if (parkingDataType == "parkingOneFree") {
    parkingOneFree = parkingData.toInt();
  }
  else if (parkingDataType == "parkingTwoFree") {
    parkingTwoFree = parkingData.toInt();
  }
  Serial.println("Data Received");
  Serial.println(parkingDataType);
  Serial.println(parkingData);

}

void setup() {
  pinMode(toArduinoPin, OUTPUT);

  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected. IP address:");
  Serial.println(WiFi.localIP());
  //initMQTT();
  //mqttCloudClient.begin(mqttCloudServer, mqttCloudPort, ipCloudStack);
  //mqttCloudClient.connect(mqttCloudClientName, mqttCloudUsername, mqttCloudPassword);
  //MQTT_Artik_Client.begin(Artik_Cloud_Server, Artik_Cloud_Port, SSL_ESP); // Connect to Artik Server

  client.setServer(mqttCloudServer, mqttCloudPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client", mqttCloudUsername, mqttCloudPassword)) {
      Serial.println("connected");
    } else {
      Serial.print("failed with state");
      Serial.print(client.state());
      delay(2000);
    }
  }
  //client.publish("parking/parkingTwoFree", "3",1);
  client.subscribe("/v1.1/actions/7d6e199242fd43cdb8722f579228b5e6");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  delay(1000);

}

void loop() {

  client.loop();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  count = parkingOneFree + parkingTwoFree;
  display.println("Parking Spots Free: " + (String)count);
  display.display();
  //Serial.println(host);
  if (count > 0) {
    digitalWrite(toArduinoPin, HIGH);
  }
  else {
    digitalWrite(toArduinoPin, LOW);
  }

  // make sure the TCP connection is still alive.
  if (client.connected() == 0) {
    Serial.println("Disconnected. Retrying in 3 seconds...");
    delay(3000);
    return;
  }



}
