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
char ssid[] = "beanpad2.0";
char password[] = "twopeasinapod69";

// Callback function for the cloud connection
void callback(char* topic, byte* payload, unsigned int length);

// Function to open a container
int openContainer(String command);
int numRequests = 0;

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

  // Functions for the REST API
  rest.function("container", openContainer);
  rest.variable("numRequests", &numRequests);

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

  for( uint8_t i = 0; i < 8; i++ ) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }
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

// Opens and closes a container
int openContainer(String command)
{
  int container = command.toInt();
  container--;
  numRequests++;
  if(container >= 0 && container < 8) {
    digitalWrite(container, LOW);
    delay(1000);
    digitalWrite(container, HIGH);
    return 1;
  }
  return 0;
}
