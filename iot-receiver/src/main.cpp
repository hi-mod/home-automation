#include <Arduino.h>
#include "esp8266_mqtt.h"

void setup() {
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

  if(mqttClient->connected()) {
    if(device == NULL) {
      Serial.println("device is null.");
    } else {
      //Serial.println(device->getEventsTopic());
      mqttClient->subscribe("projects/****/topics/water-temp-pub");
    }
  }
}