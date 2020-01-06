# How I Saved Money On My Water Bill


# My Home Automation Journey

This is the story of my journey of creating a home automation system. Starting with my hot water. Later I detail how I built my solution.


# Hot Water Heater Dies

Not long ago my water tank heater started leaking, and after I got over the initial dread of dealing with that, I thought this was the perfect time to tackle a problem that has always bothered me. The shower in my master bathroom is the furthest fixture from my water heater and, as such, takes 2-3 minutes of running the hot water before it begins to get warm. This has always irritated me as I thought it was a huge waste of water when you consider the showerhead is rated at 2.5 gallons per minute. That equates to 5 to 7 gallons per day, or 1,825 to 2,555 gallons per year for just one person! This seemed wasteful to me and it only got worse when I installed my new tankless water heater. Because the footprint of the tankless water heater was smaller and could be mounted on the wall I wanted to reclaim the floor space my tank-type water was taking in the basement. Therefore I mounted it in a corner of the basement where I could easily vent it and allow the condensation from the combustion to drain into my sump pump pit. The problem was that this added about 10 or 12 more feet of ¾” pipe from the hot water heater and all the fixtures in the house. Further increasing the time and amount of water wasted before getting hot water.


# The Solution

I decided, along with the hot water heater, I would install a water pump near the water heater unit. At the sink in the master bedroom, I installed a valve that allowed water from the hot side to be pumped into the cold water side while the water was cold. However, in order for this solution to work, I needed a way to activate the pump on demand and a way to shut the pump off once hot water reaches the fixture.

I happened to already have 3 esp8266 Arduinos (I’ve always thought that the Arduinos are cool but I’ve never really had a good reason to do anything productive with them until now), switches and breadboard already on hand. I wanted to keep the cost as low as possible and use what I already had on hand; since there are already solutions that exist that can do all or most of what I want.


# Cloud IoT Solution

Originally I wanted to do the project in Azure, but Azure IoT Hub doesn’t support sending messages to a device via the MQTT protocol, I could not find libraries for the AMPQ protocol for the Arduino platform and after doing some research it seemed the standard protocol for home automation devices was MQTT. At this point, I looked at Google Cloud and found I was able to send as well as receive MQTT messages. I had been wanting to take a look at Google Cloud for a while and this was the perfect excuse to do so.


# Reader Expectations

I am assuming you are familiar with Arduino development and tinkering. I will only be explaining key differences between PlatformIO and the Arduino development environment. If you are new to Arduino projects and development I recommend you start [here](https://www.arduino.cc/en/Guide/HomePage) first.


# Setting Up the Development Environment



1. Download and install the [Arduino development environment](https://www.arduino.cc/en/Main/Software) (We won’t actually do any development in it, but it is required by PlatformIO.)
2. Download and install [Visual Studio Code](https://code.visualstudio.com/).
3. Once Visual Studio Code is installed open it and perform any updates it wants to do and then install the PlatformIO extension. We will be doing all of our development in Visual Studio Code with the help of this extension. We will have a much richer experience here as we will get things like code completion and be able to see the overloads of methods we are calling.


# Developing The Code


## What Do We Need To Build This Solution



1. We will need 2 [ESP8266 modules](https://www.amazon.com/HiLetgo-Internet-Development-Wireless-Micropython/dp/B081CSJV2V). (We can prototype with one module initially.)
2. A breadboard for prototyping.
3. Breadboard friendly wire.
4. A momentary switch with a built-in LED that can be controlled separately from the button.
5. A resistor.
6. A relay module.


## Start Building

The diagram below is how we will wire up the relay and button. Some of what we are doing here will be changed later as we will eventually have to separate the relay and button, but this allows us to build out everything on one device. I think it will become more clear as the build proceeds. 

The yellow wire is connected to a GPIO pin on the board, the red is connected 3v power and the black to ground. You will also need to hook up the water pump somehow to the terminals on the relay. For my setup, I put my relay into a 3D printed enclosure to help minimize the exposure to mains voltage. I took an old damaged extension cord and cut a 6-foot length and added a new female plug to the end (the male plug was still good). In the middle of the cord, I carefully exposed the individual wires being careful not to cut the ground or the neutral wires. I did cut the hot wire and stripped back a length of wire shielding to expose the wires themselves.  I then placed one of the hot wires in the NO (normally open) and the other on the C (closed) terminal. This ensures when the relay doesn’t have power whatever is plugged into the extension cord will not be on.

Again referring to the diagram below, we place a momentary button on the breadboard connecting one side to the ground and the other to a GPIO pin on the board.



<p id="gdcalert1" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline drawings not supported directly from Docs. You may want to copy the inline drawing to a standalone drawing and export by reference. See <a href="https://github.com/evbacher/gd2md-html/wiki/Google-Drawings-by-reference">Google Drawings by reference</a> for details. The img URL below is a placeholder. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert2">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![drawing](https://docs.google.com/a/google.com/drawings/d/12345/export/png)

Open Visual Studio Code, by default, if you’ve installed the PlatformIO extension the PIO Home tab will open choose New Project and give it a name, board type, framework and then choose finish. See _figure 2_.

You will have your project created for you and our code will go into main.cpp under the src folder. _See figure 3._

Your main.cpp file should look like this:


```
#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}
```


Let's make our button actuate the relay:


```
#include <Arduino.h>

const int button_pin = 0;
const int button_led_pin = 16;
const int relay_pin = 5;

void setup() {
  pinMode(button_pin, INPUT_PULLUP);

  digitalWrite(button_led_pin, LOW);
  pinMode(button_led_pin, OUTPUT);

  digitalWrite(relay_pin, HIGH);
  pinMode(relay_pin, OUTPUT);

  Serial.begin(115200);
}

void loop() {
  if (digitalRead(button_pin) == LOW) {
    Serial.println("Turn the pump on...");
    digitalWrite(relay_pin, LOW);
  } else {
    Serial.println("Turn the pump off...");
    digitalWrite(relay_pin, HIGH);
  }
}
```


In the code block above we declare constants for pins we are going to use for the button, LED and relay. You’ll notice in the setup method I am writing to the relay_pin setting it HIGH before calling pinMode to setup it up for OUTPUT. I was having an issue where without this call the relay would actuate the water pump whenever the device was starting up and it would stay on. Obviously not the desired effect.

In the root of your project, you will find a file called platformio.ini.


```
;PlatformIO Project Configuration File
;
;   Build options: build flags, source filter
;   Upload options: custom upload port, speed and extra flags
;   Library options: dependencies, extra library storages
;   Advanced options: extra scripting
;
; Please visit documentation for the other options and examples
; https://docs.platformio.org/page/projectconf.html

[env:esp12e]
platform = espressif8266
board = esp12e
framework = arduino
```


Add the following lines directly after framework = arduino:


```
monitor_speed = 115200
upload_speed = 512000
lib_deps =
  Google Cloud IoT Core JWT
  MQTT
  ArduinoJson
```


These lines will make, in order, the serial monitor automatically start at baud rate 115,200, the upload speed of our code to the device at a baud rate of 512,000 and, finally, add the needed libraries for working with Google Cloud IoT.



*   [Google Cloud IoT Core JWT](https://platformio.org/lib/show/5372/Google%20Cloud%20IoT%20Core%20JWT) for connecting to Google’s IoT Core
*   [MQTT](https://platformio.org/lib/show/617/mqtt) the MQTT library that Google Cloud IoT Core JWT leverages
*   [ArduinoJson](https://platformio.org/lib/show/64/ArduinoJson) a library making it easier to serialize and deserialize JSON.

Deploy the code to your board by clicking the 

<p id="gdcalert2" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/Home-Automation0.png). Store image on your image server and adjust path/filename if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert3">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/Home-Automation0.png "image_tooltip")
 in the status bar of Visual Studio Code. 

<p id="gdcalert3" ><span style="color: red; font-weight: bold">>>>>>  gd2md-html alert: inline image link here (to images/Home-Automation1.png). Store image on your image server and adjust path/filename if necessary. </span><br>(<a href="#">Back to top</a>)(<a href="#gdcalert4">Next alert</a>)<br><span style="color: red; font-weight: bold">>>>>> </span></p>


![alt_text](images/Home-Automation1.png "image_tooltip")


OK. So now if we deploy this code to the board we will have a system that turns on the device connected to the relay when we press the button. As soon as we release the button it will turn off the connected device.

You might be saying so where is the cloud piece? This isn’t doing what you advertised. To which I say patience my young padawan. I believe in an iterative approach to development. If we just jump in and try to write the whole thing at once it can be difficult to troubleshoot what is wrong if there are problems, and trust me there will be problems! If you are growing impatient and just want the end result feel free to jump to the end of this blog post.


## Adding A Bit of Cloud


### Creating the Google Cloud IoT Core



1. In the Cloud Console, on the [project selector page](https://console.cloud.google.com/projectselector2/home/dashboard), select or create a Google Cloud project. \
Note: If you don't plan to keep the resources that you create in this procedure, create a project instead of selecting an existing project. After you finish these steps, you can delete the project, removing all resources associated with the project.
2. Make sure that billing is enabled for your Google Cloud project. [Learn how to confirm billing is enabled for your project](https://cloud.google.com/billing/docs/how-to/modify-project).
3. Enable the [Cloud IoT Core and Cloud Pub/Sub APIs](https://console.cloud.google.com/projectselector2/home/dashboard).


## Set up your local environment and install prerequisites



1. [Install and initialize the Cloud SDK](https://cloud.google.com/sdk/docs/). Cloud IoT Core requires version 173.0.0 or higher of the SDK.
2. Set up a [Node.js development environment](https://cloud.google.com/nodejs/docs/setup). \
Alternatively, you can use [Google Cloud Shell](https://cloud.google.com/shell/docs/starting-cloud-shell), which comes with Cloud SDK and Node.js already installed.


## Create a device registry



1. Go to the [Google Cloud IoT Core](https://console.cloud.google.com/iot) page in Cloud Console.
2. Click **Create registry.**
3. Enter my-registry for the **Registry ID**.
4. If you're in the US, select **us-central1** for the **Region**. If you're outside the US, select your preferred [region](https://cloud.google.com/iot/docs/requirements#cloud_regions).
5. Select **MQTT** for the **Protocol**.
6. In the **Default telemetry topic** dropdown list, select **Create a topic.**
7. In the **Create a topic** dialog, enter my-device-events in the **Name** field.
8. Click **Create** in the **Create a topic** dialog.
9. The **Device state topic** and **Certificate value** fields are optional, so leave them blank.
10. Click **Create** on the Cloud IoT Core page.

You've just created a device registry with a Cloud Pub/Sub topic for publishing device telemetry events.


## Upload the Cloud Function

We need a cloud function that will take messages published by one of the esp8266 boards and relay it to the other esp8266 board. Here is the function:

index.js


```
'use strict';
const {google} = require('googleapis');

const projectId = [YOUR_PROJECT_ID];
const cloudRegion = [YOUR_REGION];

exports.relayCloudIot = (event, callback) => {
  let record;
  try {
    record = JSON.parse(
      event.data
        ? Buffer.from(event.data, 'base64').toString()
        : '{}');
    console.log(record);
  } catch (error) {
    console.log("Could not convert event.data to JSON.");
    console.log(error);
    return;
  }

  console.log(`${record.timestamp} ${record.deviceId} ${record.registryId}`);

  const config = {
    deviceId: record.deviceId,
    pump: record.pump,
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
  });
};
```


package.json


```
{
  "name": "home-automation",
  "version": "1.0.0",
  "description": "",
  "main": "index.js",
  "engines": {
    "node": ">=4.3.2"
  },
  "dependencies": {
    "googleapis": ">=32.0.0"
  }
}
```


Let’s go ahead and deploy that function, the command below will deploy everything in your folder to a staging storage bucket and then to your Cloud Function:


```
gcloud functions deploy relayCloudIot \
    --runtime nodejs8 \
    --stage-bucket=gs://your-gcs-bucket \
    --trigger-topic=[YOUR_TOPIC] \
    --project=[PROJECT_ID]
```


Now we need to add a couple devices:

The following command will generate an RSA-256 keypair, run it twice to generate 2 keypairs; changing the name of the file it outputs for the second run:


```
openssl req -x509 -newkey rsa:2048 -days 3650 -keyout rsa_private.pem -nodes -out \
    rsa_cert.pem -subj "/CN=unused"
```


After you have generated the keypairs, use the public keys to register your devices, again running this command twice; changing the name of your device and using the other public key:


```
gcloud iot devices create \
    --registry=[YOUR_REGISTRY_ID] \
    --region "us-central1" \
    --public-key path=rsa_cert.pem,type=rs256 [YOUR_DEVICE_ID]
```



## Let’s Get Those Devices Connected

In order for the esp8266 to connect to the IoT Hub Core we will have to upload some certificates to the firmware. Here are the steps:



1. Create data folder (it should be on the same level as src folder) and put [these](https://github.com/GoogleCloudPlatform/google-cloud-iot-arduino/tree/master/examples/Esp8266-lwmqtt/data) files here.
2. Run “Upload File System image” task in [PlatformIO IDE](http://docs.platformio.org/en/latest/ide/pioide.html#pioide) or use [PlatformIO Core (CLI)](http://docs.platformio.org/en/latest/core.html#piocore) and [platformio run --target](http://docs.platformio.org/en/latest/userguide/cmd_run.html#cmdoption-platformio-run-t) command with uploadfs target.

The [iot-button-press folder](https://github.com/hi-mod/home-automation/tree/master/iot-cloud-functions/relayCloudIot) of the tutorial contains the code that you will deploy to both esp8266 devices. I designed it in such a way that I could deploy the same code base to both devices. I will give a brief overview of each file and changes you will need to make in order to get it to work for you.


#### config.h

This file contains global constants for the program, several that you will need to change:



*   ssid
*   password
*   project_id
*   location
*   register_id
*   device_id: this needs to be the name of the device in IoT Hub Core that the esp8266 will connect to, this and private_key_str are the only sections of code that will need to be modified between deployments of each esp8266.
*   private_key_str: this is the private key for the device we registered on IoT Hub Core. Run `openssl ec -in <private-key.pem> -noout -text `and copy the priv: part.


#### esp8266_mqtt.h

You won’t need to change anything here, but I thought I would give an overview of what is happening here:



*   getJwt(): makes a call to Google Cloud library to request a JWT token.
*   setupCert(): loads the certificates we uploaded to each esp8266 for authenticating to IoT Hub Core.
*   setupCloudIoT(): sets up the WiFi, registers the esp8266 with the device on IoT Hub Core, sets up the certificates and configures the MQTT client for communication.


#### main.cpp

Here you will need to make a couple modifications:



*   Constants
    *   button_pin is a constant to the GPIO the button is connected to.
    *   button_led_pin is a constant to the GPIO the button’s led is connected to.
    *   relay_pin is a constant to the GPIO the relay is connected to.
    *   pump_duration is the number of seconds to run the pump for.
    *   ignore_pump_on_duration is the number of seconds to ignore older pump on messages
    *   pump_device the name of the device handling the water pump
    *   water_temp_device the name of the device handling the button and led
*   Variables
    *   pumpOn a boolean that tells us whether the pump is on or not.
    *   pumpStartTime the time that we turned the pump on. This is used to determine how long the pump has been on.
*   Methods
    *   messageReceived() is the event handler called every time a message received from IoT Hub Core.
    *   setup() sets up the pins to be used with the button, led and pump.
    *   loop() processes incoming messages, sends messages to pump and itself when the button is pushed. Turns the pump off if it has been on for the duration the pump should be on for.

Deploy your code to each device. Once deployed when you press the button 2 messages are sent to each configured device in IoT Hub Core our Cloud Function receives these messages, because it is subscribed to the topic that are messages are published to, and forwards them to appropriate devices. The esp8266 that you pressed the button on receives a message and it’s LED is lit up. The esp8266 that controls the relay receives a message and the relay is actuated.

Congrats! Now go forth and automate your home. My next steps will be to add a way to turn the water pump on from my phone or website. I would also like to know if my garage door is open and be able to close or open it remotely.

Thanks for reading!

Aaron
