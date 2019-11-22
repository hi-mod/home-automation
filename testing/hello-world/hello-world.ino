// Libraries
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// WiFi parameters
#define WLAN_SSID "FBI Van"
#define WLAN_PASS "****"

const char* MQTT_SERVER = "****.azure-devices.net";
const int MQTT_SERVER_PORT = 8883;
const char* MQTT_SERVER_USERNAME = "****.azure-devices.net/****";
const char* MQTT_SERVERKEY = "****";

const char* IOTHUB_DEVICEID = "****";

const int MESSAGE_MAX_LEN = 256;

const int buttonPin = 0;

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClientSecure wclient;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
PubSubClient client(wclient, MQTT_SERVER, MQTT_SERVER_PORT);

void setup()
{
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println(F("Adafruit IO Example"));

  // Connect to WiFi access point.
  Serial.println();
  Serial.println();
  delay(10);
  Serial.print(F("Connecting to "));
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();

  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());

  connect();
}

void loop()
{
  if (digitalRead(buttonPin) == LOW)
  {
    char messagePayload[MESSAGE_MAX_LEN];
    readMessage(1, messagePayload);
    Serial.println(messagePayload);
    if (client.publish("devices/temp-sensor/messages/events/", messagePayload))
    {
      Serial.println("message sent...");
    }
    else
    {
      Serial.println("message failed to send...");
    }
  }
}

// connect to adafruit io via MQTT
void connect()
{
  Serial.println("Connecting to MQTT server");
  if (client.connect(MQTT::Connect(IOTHUB_DEVICEID).set_auth(MQTT_SERVER_USERNAME, MQTT_SERVERKEY)))
  {
    Serial.println("Connected to MQTT server");
  }
}

void readMessage(int messageId, char* messagepayload)
{
  StaticJsonDocument<MESSAGE_MAX_LEN> doc;
  JsonObject message = doc.createNestedObject("message");
  JsonObject sysProps = message.createNestedObject("systemProperties");
  sysProps["contentType"] = "application/json";
  sysProps["contentEncoding"] = "UTF-8";
  message["body"] = "{\"deviceId\":\"Feather HUZZAH ESP8266 WiFi\",\"messageId\":1,\"temperature\":72,\"humidity\":50}";
  serializeJson(doc, messagepayload, MESSAGE_MAX_LEN);
}
