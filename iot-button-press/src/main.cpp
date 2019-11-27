#include <Arduino.h>
#include "esp8266_mqtt.h"
#include <time.h>
#include <ArduinoJson.h>

const int button_pin = 0;
const int relay_pin = 5;
const int pump_duration = 75;
const int ignore_pump_on_duration = 75;

bool pumpOn = false;
ulong pumpStartTime;

void messageReceived(String &topic, String &payload)
{
  Serial.println("incoming: " + topic + " - " + payload);
  if(pumpOn) {
    Serial.println("Pump is already on...");
    return;
  }
  DynamicJsonDocument doc(1024);

  deserializeJson(doc, payload);
  JsonObject obj = doc.as<JsonObject>();

  if(obj["pump"] == "on") {
    time_t rawtime;
    time(&rawtime);
    if(rawtime - String(obj["millis"].as<String>()).toInt() >= ignore_pump_on_duration) {
      Serial.println("Pump on message is to old, ignoring...");
      return;
    }
    pumpOn = true;
    pumpStartTime = rawtime;
    Serial.println("Turn the pump on...");
    digitalWrite(relay_pin, LOW);
  }
}

void setup() {
  pinMode(button_pin, INPUT_PULLUP);

  digitalWrite(relay_pin, HIGH);
  pinMode(relay_pin, OUTPUT);

  Serial.begin(115200);

  Serial.println("Connecting to MQTT server");
  setupCloudIoT(); // Creates globals for MQTT

  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());
}

void loop() {
  mqttClient->loop();
  delay(10); // <- fixes some issues with WiFi stability

  if (!mqttClient->connected())
  {
    ESP.wdtDisable();
    connect();
    ESP.wdtEnable(0);
  }

  char buffer[80];
  time_t rawtime;
  struct tm * timeinfo;
  if (digitalRead(button_pin) == LOW) {
    time(&rawtime);
    timeinfo = localtime (&rawtime);
    strftime(buffer,80,"%Y-%m-%d %H:%M:%S",timeinfo);

    String messagePayload = "{\"deviceId\":\"" + String(device_id) + "\",\"timestamp\":\"" + String(buffer) + "\",\"millis\":" 
      + rawtime + ",\"pump\":\"on\",\"registryId\":\"" + String(registry_id) + "\",\"cloudRegion\":\"" + String(location) + "\"}";
    Serial.println(messagePayload);
    publishTelemetry(messagePayload);
    delay(1000);
  }

  if(pumpOn) {
    time(&rawtime);
    if(rawtime - pumpStartTime >= pump_duration) {
      Serial.println("\nTurn the pump off...");
      pumpOn = false;
      digitalWrite(relay_pin, HIGH);
    }
  }
}