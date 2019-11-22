#include <Arduino.h>
#include "esp8266_mqtt.h"
#include <time.h>

const int MESSAGE_MAX_LEN = 256;

const int buttonPin = 0;

int messageId = 1;

void setup() {
  Serial.begin(115200);

  Serial.println("Connecting to MQTT server");
  setupCloudIoT(); // Creates globals for MQTT

  pinMode(buttonPin, INPUT_PULLUP);

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

  if (digitalRead(buttonPin) == LOW) {
    //char messagePayload[100];

    char buffer[80];
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    strftime (buffer,80,"%Y-%m-%d %H:%M:%S",timeinfo);

    char* messagePayload = "{\"cloudRegion\":\"us-central1\",\"deviceId\":\"water-temp-dev\",\"registryId\":\"home-automation-registry\",\"hops\":2}";
    Serial.println(messagePayload);
    publishTelemetry(messagePayload);
    delay(1000);
  }
}