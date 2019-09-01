#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti WiFiMulti;
const uint16_t port = 80;
const char * host = "192.168.1.6";

const int transmitPinOne = 4; 
const int transmitPinTwo = 13; 
const int receivePinOne = 16; 
const int receivePinTwo = 12; 
const int statLED = 5; 

int irResp = 0;
int led = 1;
int numChecks = 50; 
int personCount = 0;
long int currentTime; 

bool lightsOn = false; 


void setup()
{

  Serial.begin(115200);

  // Connecting to WiFi....
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Sparkle Mausoleum", "saile123"); 
  WiFiClient client; 

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

 // Serial.print("Connecting to Relay Server...");
 // Serial.println(host);
  
  while(!client.connect(host, port)) {
    Serial.println("."); 
    delay(500); 
  }

  Serial.println("Connected!"); 
  client.println("/relay/1");
  Serial.println("Read from server: "); 
  String line = client.readStringUntil('\r'); 
  Serial.println(line);

  pinMode(transmitPinOne, OUTPUT); 
  pinMode(transmitPinTwo, OUTPUT); 
  pinMode(receivePinOne, INPUT);
  pinMode(receivePinTwo, INPUT);
  digitalWrite(receivePinOne, HIGH); 
  digitalWrite(receivePinTwo, HIGH); 
  pinMode(statLED, OUTPUT);
  
  delay(500);

}
void loop()
{
  currentTime = millis(); // Keep time up to date. 
  digitalWrite(statLED, HIGH); 

  irResp = IR_stream(led); 
  Serial.println(irResp);
  if(irResp == 1) {
    currentTime = millis();  
    Serial.println("One"); 
    led++;
    while( millis() - currentTime < 2000){
      if(IR_stream(led) == 2){
        lightsOn = true; 
        personCount++; 
        Serial.print("Person Count: "); 
        Serial.println(personCount);
        digitalWrite(statLED, LOW); 
        sendRelayOn();
        delay(500); //debounce of sorts
        break;
      }
    }
  }
  else if(irResp == 2 && lightsOn == true) { 
    currentTime = millis();  
    led--;
    while( millis() - currentTime < 2000){
      if(IR_stream(led) == 1){
        if (personCount > 0)
          personCount--;
        lightsOn = false; 
        Serial.print("Person Count: "); 
        Serial.println(personCount);
        digitalWrite(statLED, LOW); 
        sendRelayOff();
        delay(500); //debounce of sorts
      }
    }
  }

  delay(1);

  if (led == 2)
    led = 1;  
  else 
    led++;
}

int IR_stream(int LED){

  for (int i = 0; i < numChecks; i ++) {

    switch(LED) {
      case 1:
        digitalWrite(transmitPinOne, HIGH); 
        break;
      case 2:
        digitalWrite(transmitPinTwo, HIGH); 
        break;
      
    }

    delayMicroseconds(6); 

    switch(LED) {
      case 1:
        digitalWrite(transmitPinOne, LOW); 
        break;
      case 2:
        digitalWrite(transmitPinTwo, LOW); 
        break;
    }

    delayMicroseconds(17); 
  }

  if(LED == 1) {
    if(digitalRead(receivePinOne) == HIGH) 
      return(1); 
  }

  if(LED == 2) {
    if(digitalRead(receivePinTwo) == HIGH) 
      return(2); 
  }

  return(0);    

}

int sendRelayOn(){

    WiFiClient client; 
    if(!client.connect(host, port)) 
      Serial.println("Could not connect!"); 
    client.println("/relay/1");
    client.stop();
    //Serial.println("Read from server: "); 
    //String line = client.readStringUntil('\r'); 
    //Serial.println(line);
} 

int sendRelayOff(){
    WiFiClient client; 
    if(!client.connect(host, port)) 
      Serial.println("Could not connect!"); 
    client.println("/relay/2");
    client.stop();
    //Serial.println("Read from server: "); 
    //String line = client.readStringUntil('\r'); 
    //Serial.println(line);
}
