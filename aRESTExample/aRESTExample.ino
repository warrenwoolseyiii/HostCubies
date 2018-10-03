/*
  This a simple example of the aREST Library for the Arduino/Genuino MKR1000 board.
  See the README file for more details.

  Written in 2016 by Marco Schwartz under a GPL license.
*/

// Import required libraries
#include <SPI.h>
#include <WiFi101.h>
#include <PubSubClient.h>
#include <aREST.h>

// Status
int status = WL_IDLE_STATUS;

// Clients
WiFiClient wifiClient;
PubSubClient client(wifiClient);
String local_ip = "";

// Create aREST instance
aREST rest = aREST(client);

// Device ID for the device on the cloud (should be 6 characters long)
char * device_id = "fa3698";

// WiFi parameters
char ssid[] = "Flume-Pilot2";
char password[] = "esponly1";

// Callback function for the cloud connection
void callback(char* topic, byte* payload, unsigned int length);

// The port to listen for incoming TCP connections
#define LISTEN_PORT           80

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

void setup(void)
{
  // Start Serial
  Serial.begin(115200);

  // Set callback
  client.setCallback(callback);

  // Give name and ID to device (ID should be 6 characters long)
  rest.set_id(device_id);
  rest.set_name("mkr1000");

  // Connect to WiFi
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, password);

    // Wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Local server started on IP:");

  // Print the IP address
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
  local_ip = ipToString(WiFi.localIP());

}

void loop() {

  // Connect to the cloud
  rest.handle(client);

  // Handle REST calls
  WiFiClient localClient = server.available();
  if (!localClient) {
    return;
  }
  while(!localClient.available()){
    delay(1);
  }
  rest.handle(localClient);

}

// Handles message arrived on subscribed topic(s)
void callback(char* topic, byte* payload, unsigned int length) {

  // Handle
  rest.handle_callback(client, topic, payload, length);

}

// Convert IP address to String
String ipToString(IPAddress address)
{
  return String(address[0]) + "." +
    String(address[1]) + "." +
    String(address[2]) + "." +
    String(address[3]);
}
