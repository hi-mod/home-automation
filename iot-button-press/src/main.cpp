#include <Arduino.h>
#include "esp8266_mqtt.h"
#include <time.h>
#include <ArduinoJson.h>

const int button_pin = 0;
const int button_led_pin = 16;
const int relay_pin = 5;
const int pump_duration = 240; // default pump duration in seconds.
const int ignore_pump_on_duration = 240; // how long ago to respond to a on messae to listen in seconds.
const String pump_device = "water-pump-device";
const String water_temp_device = "water-temp-device";

bool pumpOn = false;
ulong pumpStartTime;
ushort pumpDuration;

void setup() {
  pinMode(button_pin, INPUT_PULLUP);

  digitalWrite(button_led_pin, LOW);
  pinMode(button_led_pin, OUTPUT);

  digitalWrite(relay_pin, HIGH);
  pinMode(relay_pin, OUTPUT);

  Serial.begin(115200);

  Serial.println("Connecting to MQTT server");
  setupCloudIoT(); // Creates globals for MQTT

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void sendMessages(time_t rawtime, String pumpState, ushort pumpDuration = pump_duration) {
  char buffer[80];
  struct tm * timeinfo;
  timeinfo = localtime(&rawtime);
  strftime(buffer,80,"%Y-%m-%d %H:%M:%S",timeinfo);

  String messagePayload = "{\"deviceId\":\"" + pump_device + "\",\"timestamp\":\"" + String(buffer) + "\",\"millis\":" + rawtime + ",\"pumpState\":\""
    + pumpState + "\",\"pumpDuration\":\"" + pumpDuration + "\",\"registryId\":\"" + String(registry_id) + "\",\"cloudRegion\":\"" + String(location) + "\"}";
  Serial.println(messagePayload);
  publishTelemetry(messagePayload);
  messagePayload = "{\"deviceId\":\"" + water_temp_device + "\",\"timestamp\":\"" + String(buffer) + "\",\"millis\":" + rawtime + ",\"pumpState\":\""
    + pumpState + "\",\"pumpDuration\":\"" + pumpDuration + "\",\"registryId\":\"" + String(registry_id) + "\",\"cloudRegion\":\"" + String(location) + "\"}";
  Serial.println(messagePayload);
  publishTelemetry(messagePayload);
}

void turnOnPump(time_t rawtime, ushort _pumpDuration = pump_duration) {
  pumpOn = true;
  pumpStartTime = rawtime;
  pumpDuration = _pumpDuration;
  Serial.println("Turn the pump on...");
  digitalWrite(relay_pin, LOW);
  digitalWrite(button_led_pin, HIGH);
}

void turnOffPump() {
  Serial.println("\nTurn the pump off...");
  pumpOn = false;
  digitalWrite(relay_pin, HIGH);
  digitalWrite(button_led_pin, LOW);
}

void messageReceived(String &topic, String &payload)
{
  Serial.println("incoming: " + topic + " - " + payload);
  DynamicJsonDocument doc(1024);

  deserializeJson(doc, payload);
  JsonObject obj = doc.as<JsonObject>();

  String pumpState = obj["pumpState"];
  if(pumpState == "on") {
    if(pumpOn) {
      Serial.println("Pump is already on...");
      return;
    }
    time_t rawtime;
    time(&rawtime);
    if(rawtime - obj["millis"].as<String>().toInt() >= ignore_pump_on_duration) {
      Serial.println("Pump on message is to old, ignoring...");
      return;
    }
    pumpDuration = obj.containsKey("pumpDuration") ? obj["pumpDuration"].as<String>().toInt() : pump_duration;
    turnOnPump(rawtime, pumpDuration);
  } else if(pumpState == "off") {
    turnOffPump();
  } else {
    Serial.println("Unexpected pump state value");
  }
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

  time_t rawtime;

  // if button_pin becomes low they pressed the button.
  // send messages to turn the pump on.
  if (digitalRead(button_pin) == LOW) {
    time(&rawtime);
    sendMessages(rawtime, "on");
    delay(1000);
  }

  if(pumpOn) {
    time(&rawtime);
    if(rawtime - pumpStartTime >= pumpDuration) {
      turnOffPump();
      sendMessages(rawtime, "off", 1);
    }
  }
}