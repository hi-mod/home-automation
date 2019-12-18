"use strict";
const {google} = require('googleapis');

const projectId = '****';
const cloudRegion = '****';
const registryId = "****";

exports.activateWaterPump = (req, res) => {
  const date = new Date();

  const config = {
    deviceId: "water-pump-device",
    pump: "on",
    timestamp: `${date.getFullYear()}-${date.getMonth()}-${date.getDate()} ${date.getHours()}:${date.getMinutes()}:${date.getSeconds()}`,
    millis: Math.round(date / 1000)
  };

  google.auth.getClient().then(client => {
    google.options({
      auth: client
    });
    console.log('START setDeviceConfig');
    const parentName = `projects/${projectId}/locations/${cloudRegion}`;
    const registryName = `${parentName}/registries/${registryId}`;
    let binaryData = Buffer.from(JSON.stringify(config)).toString('base64');
    let request = {
      name: `${registryName}/devices/${config.deviceId}`,
      versionToUpdate: 0,
      binaryData: binaryData
    };
    console.log('Set device config.');
    google.cloudiot('v1').projects.locations.registries.devices.modifyCloudToDeviceConfig(request);
    var message = "Sent: " + JSON.stringify(config);

    config.deviceId = "water-temp-device";
    binaryData = Buffer.from(JSON.stringify(config)).toString('base64');
    request = {
      name: `${registryName}/devices/${config.deviceId}`,
      versionToUpdate: 0,
      binaryData: binaryData
    };
    console.log('Set device config.');
    google.cloudiot('v1').projects.locations.registries.devices.modifyCloudToDeviceConfig(request);
    message += "\nSent: " + JSON.stringify(config);
    res.send(message);
  }).then(result => {
    if(!result) {
      console.log("result in undefined in second then()")
    }
    console.log(result);
  });
};