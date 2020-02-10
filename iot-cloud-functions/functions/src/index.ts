import * as functions from "firebase-functions";
import { google } from "googleapis";
import * as admin from "firebase-admin";
import { PubSub } from "@google-cloud/pubsub";

admin.initializeApp({
  credential: admin.credential.applicationDefault(),
  databaseURL: 'https://home-automation-hi-mod.firebaseio.com'
});

const projectId = "home-automation-hi-mod";
const cloudRegion = "us-central1";
const registryId = "home-automation-registry";

interface DeviceRecord {
  deviceId: string;
  pumpState: string;
  pumpDuration: number;
  timestamp: string;
  millis: number;
  registryId: string;
  cloudRegion: string;
}

function convertToJSON(str: string) {
  try {
    return JSON.parse(str);
  } catch (error) {
    console.log("Could not convert event.data to JSON.");
    return;
  }
}

export const relayCloudIot = functions.pubsub
  .topic("water-temp-topic")
  .onPublish(async (event) => {
    console.log(`Event: ${JSON.stringify(event)}`);
    let record: DeviceRecord;
    const eventData = event.data ? Buffer.from(event.data, "base64").toString() : "{}";
    console.log(`eventData: ${eventData}`);
    record = convertToJSON(eventData);
    if(!record) return;
    console.log(record);

    const client = await google.auth.getClient();
    google.options({
      auth: client
    });
    console.log("START setDeviceConfig");
    const parentName = `projects/${projectId}/locations/${record.cloudRegion}`;
    const registryName = `${parentName}/registries/${record.registryId}`;
    const binaryData = Buffer.from(JSON.stringify(record)).toString("base64");
    const request = {
      name: `${registryName}/devices/${record.deviceId}`,
      versionToUpdate: 0,
      binaryData: binaryData
    };
    console.log("Set device config.");
    await google
      .cloudiot("v1")
      .projects.locations.registries.devices.modifyCloudToDeviceConfig(request);
  });

export const actuateWaterPump = functions.https.onRequest(async (request: functions.Request, response: functions.Response) => {
  const date = new Date();

  const pumpState = request.query.pumpState;
  console.info(`pumpState: ${pumpState}`);
  const pumpDuration = request.query.pumpDuration;
  console.info(`pumpDuration: ${pumpDuration}`);

  const config = {
    deviceId: "water-pump-device",
    pumpState,
    pumpDuration,
    timestamp: `${date.getFullYear()}-${date.getMonth()}-${date.getDate()} ${date.getHours()}:${date.getMinutes()}:${date.getSeconds()}`,
    millis: Math.round(date.valueOf() / 1000),
    cloudRegion: cloudRegion,
    registryId: registryId
  };

  await publishMessage("water-temp-topic", JSON.stringify(config));
  let message = "Sent: " + JSON.stringify(config);

  config.deviceId = "water-temp-device";
  await publishMessage("water-temp-topic", JSON.stringify(config));
  message += "\nSent: " + JSON.stringify(config);
  response.send(message);
});

async function publishMessage(topicName: string, data: string) {
  const pubSubClient = new PubSub({
    projectId
  });
  const messageId = await pubSubClient.topic(topicName).publish(Buffer.from(data));
  console.log(`Message ${messageId} published.`);
}