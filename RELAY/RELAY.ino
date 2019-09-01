/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>
#include <Wire.h>

#define RELAY_ADDR 0x6D
#define TOGGLE_RELAY_ONE 0x01
#define TOGGLE_ALL_OFF 0xA

const char* ssid = "Sparkle Mausoleum";
const char* password = "saile123";
const uint8_t LED = 4; 

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  delay(10);

  // prepare GPIO2
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  uint8_t relayOn;
  if (req.indexOf("/relay/1") != -1) {
    if(relayOn == 1)
      return; 
    Wire.beginTransmission(RELAY_ADDR);
    Wire.write(TOGGLE_RELAY_ONE);
    Wire.endTransmission();
    relayOn = 1;  
    digitalWrite(LED, HIGH); 
    delay(500); 
    digitalWrite(LED, LOW); 
  }
  else if (req.indexOf("/relay/2") != -1) {
    Wire.beginTransmission(RELAY_ADDR);
    Wire.write(TOGGLE_ALL_OFF);
    Wire.endTransmission();
    relayOn = 0;  
    digitalWrite(LED, HIGH); 
    delay(500); 
    digitalWrite(LED, LOW); 
  }
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  // Set GPIO2 according to the request

  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nRelay is ";
  s += (relayOn) ? "high" : "low";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}
