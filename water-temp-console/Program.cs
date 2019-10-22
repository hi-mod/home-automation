using Microsoft.Azure.EventHubs;
using Microsoft.Azure.EventHubs.Processor;
using Microsoft.Extensions.Configuration;
using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Unosquare.RaspberryIO;
using Unosquare.RaspberryIO.Gpio;

namespace water_temp_console
{
    public class WaterTempReader
    {
        public static void Main(string[] args)
        {
            MainAsync(args).GetAwaiter().GetResult();
        }

        public static async Task MainAsync(string[] args)
        {
            Console.WriteLine("Registering EventProcessor...");

            var appDir = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);

            var builder = new ConfigurationBuilder()
                .SetBasePath(appDir)
                .AddJsonFile("appsettings.json")
                .AddEnvironmentVariables();

            var config = builder.Build();

            var eventHubName = config["appSettings:event-hub-name"];
            var waterTempEventHub = config.GetConnectionString("water-temp-event-hub");
            var waterTempBlobStorage = config.GetConnectionString("water-temp-blob-storage");
            var storageAccountName = config["appSettings:storageAccountName"];

            var eventProcessorHost = new EventProcessorHost(
                eventHubName,
                PartitionReceiver.DefaultConsumerGroupName,
                waterTempEventHub,
                waterTempBlobStorage,
                storageAccountName);

            // Registers the Event Processor Host and starts receiving messages
            await eventProcessorHost.RegisterEventProcessorAsync<SimpleEventProcessor>();

            Console.WriteLine("Receiving. Press ENTER to stop worker.");
            Console.ReadLine();

            Console.WriteLine("Shutting down... please wait...");

            // Disposes of the Event Processor Host
            await eventProcessorHost.UnregisterEventProcessorAsync();
        }

        public class SimpleEventProcessor : IEventProcessor
        {
            public Task CloseAsync(PartitionContext context, CloseReason reason)
            {
                Console.WriteLine($"Processor Shutting Down. Partition '{context.PartitionId}', Reason: '{reason}'.");
                return Task.CompletedTask;
            }

            public Task OpenAsync(PartitionContext context)
            {
                Console.WriteLine($"SimpleEventProcessor initialized. Partition: '{context.PartitionId}'");
                return Task.CompletedTask;
            }

            public Task ProcessErrorAsync(PartitionContext context, Exception error)
            {
                Console.WriteLine($"Error on Partition: {context.PartitionId}, Error: {error.Message}");
                return Task.CompletedTask;
            }

            public Task ProcessEventsAsync(PartitionContext context, IEnumerable<EventData> messages)
            {
                var blinkingPin = Pi.Gpio[1];
                blinkingPin.PinMode = GpioPinDriveMode.Output;
                blinkingPin.Write(GpioPinValue.High);
                foreach (var eventData in messages)
                {
                    var data = Encoding.UTF8.GetString(eventData.Body.Array, eventData.Body.Offset,
                        eventData.Body.Count);
                    Console.WriteLine($"Message received. Partition: '{context.PartitionId}', Data: '{data}'");
                }
                blinkingPin.Write(GpioPinValue.Low);

                return context.CheckpointAsync();
            }
        }
    }
}