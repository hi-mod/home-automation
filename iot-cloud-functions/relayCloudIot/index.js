// [START iot_relay_message_js]
'use strict';
const {google} = require('googleapis');

const projectId = '****';
const cloudRegion = '****';

exports.relayCloudIot = (event, callback) => {
  console.log(event.data);
  const record = JSON.parse(
    event.data
      ? Buffer.from(event.data, 'base64').toString()
      : '{}');
  console.log(record);

  console.log(`${record.timestamp} ${record.deviceId} ${record.registryId}`);

  const config = {
    deviceId: record.deviceId,
    pump: record.pump,
    timestamp: record.timestamp,
    millis: record.millis
  };

  google.auth.getClient().then(client => {
    google.options({
      auth: client
    });
    console.log('START setDeviceConfig');
    const parentName = `projects/${projectId}/locations/${cloudRegion}`;
    const registryName = `${parentName}/registries/${record.registryId}`;
    const binaryData = Buffer.from(JSON.stringify(config)).toString('base64');
    const request = {
      name: `${registryName}/devices/${config.deviceId}`,
      versionToUpdate: 0,
      binaryData: binaryData
    };
    console.log('Set device config.');
    return google.cloudiot('v1').projects.locations.registries.devices.modifyCloudToDeviceConfig(request);
  }).then(result => {
    console.log(result);
    console.log(result.data);
  });
};
// [END iot_relay_message_js]