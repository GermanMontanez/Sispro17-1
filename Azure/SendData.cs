using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;
using Microsoft.Azure.Devices.Client;
using Newtonsoft.Json;


namespace SimulatedDevice
{
    class Program
    {
        static DeviceClient deviceClient;
        static string iotHubUri = "MarkApp.azure-devices.net"; //Change this value for your own IoTHUB URL.
        static string deviceKey = "eakY4ck3pWtSRCjdevbX/MfFtyPkI4SdgZjqGOmzwqY="; //Change this value for your own device key.
        static SerialPort _serialPort;
		static string deviceID = "myFirstDevice"; //Change this value for your device id
		


        static void Main(string[] args)
        {

            // Create a new SerialPort object with default settings.
            _serialPort = new SerialPort();

            // Allow the user to set the appropriate properties.
            _serialPort.PortName = "COM3"; //Change this value for your own serial port.
            _serialPort.BaudRate = 9600; 	//Change this value for your own BaudRate.
            _serialPort.Open();

            Console.WriteLine("Simulated device\n");
            deviceClient = DeviceClient.Create(iotHubUri, new DeviceAuthenticationWithRegistrySymmetricKey(deviceID, deviceKey), TransportType.Mqtt); 

            SendDeviceToCloudMessagesAsync();
            Console.ReadLine();
        }

        private static async void SendDeviceToCloudMessagesAsync()
        {
 
            double tempint = 0;
            double tempout = 0;
            double humy = 0;

            Random rand = new Random();
            var buffer = new char[12];

            while (true)
            {
                _serialPort.Write("T");

                _serialPort.Read(buffer, 0, 12);

                tempint = ((double)Char.GetNumericValue(buffer[1]))*10+ ((double)Char.GetNumericValue(buffer[2]))+ ((double)Char.GetNumericValue(buffer[3])) *0.1;
                tempout = ((double)Char.GetNumericValue(buffer[5])) * 10 + ((double)Char.GetNumericValue(buffer[6])) + ((double)Char.GetNumericValue(buffer[7])) * 0.1;
                humy = ((double)Char.GetNumericValue(buffer[9])) * 10 + ((double)Char.GetNumericValue(buffer[10])) + ((double)Char.GetNumericValue(buffer[11])) * 0.1;


                var telemetryDataPoint = new
                {
                    deviceId = deviceID,
                    internalTemp = tempint,
                    externalTemp = tempout,
                    humidity = humy
                };
                var messageString = JsonConvert.SerializeObject(telemetryDataPoint);
                var message = new Message(Encoding.ASCII.GetBytes(messageString));

                await deviceClient.SendEventAsync(message);
                Console.WriteLine("{0} > Sending message: {1}", DateTime.Now, messageString);

                Task.Delay(1000).Wait();
            }
        }
    }

    
}