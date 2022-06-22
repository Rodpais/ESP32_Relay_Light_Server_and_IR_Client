#include <Wire.h>
#include "SparkFun_Qwiic_Relay.h"
#include "ArduinoJson.h"

#include <ArduinoOTA.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>

#define RELAY_ADDR 0x6D
#define INCORR_PARAM 0xFF
#define SUCCESS 0x00

Qwiic_Relay quadRelay(RELAY_ADDR);


AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

String config_path = "/home.json";

// A local way to save the state of our relays without relying on the 
// serialization process of the Arduino Json library.
struct Relay_states {
  uint8_t relay_one, 
  uint8_t relay_two,
  uint8_t relay_three,
  uint8_t relay_four
};

struct Wifi_creds {
  char host[64], 
  char password[64]
};

Relay_states relay_states;
Wifi_creds wifi_creds;

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    Serial.printf("ws[%s][%u] conectado\n", server->url(), client->id());
    client->printf("Hello Client %u :)", client->id());
    client->ping();
  } else if(type == WS_EVT_DISCONNECT){
    Serial.printf("ws[%s][%u] disconectado: %u\n", server->url(), client->id());
  } else if(type == WS_EVT_ERROR){
    Serial.printf("ws[%s][%u] erro(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if(type == WS_EVT_PONG){
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA){
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    String msg = "";
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
      Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < info->len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < info->len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n",msg.c_str());

      if(info->opcode == WS_TEXT)
        client->text("Recebi sua mensagem de texto");
      else
        client->binary("Recebi sua mensagem binária");
    } else {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if(info->index == 0){
        if(info->num == 0)
          Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n",msg.c_str());

      if((info->index + len) == info->len){
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if(info->final){
          Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          if(info->message_opcode == WS_TEXT)
            client->text("Recebi sua mensagem de texto");
          else
            client->binary("Recebi sua mensagem binária");
        }
      }
    }
  }
}

// This loads the global structs that hold such information
load_wifi_settings();

Serial.println(wifi_creds.host);
Serial.println(wifi_creds.password);

const char* ssid = wifi_creds.host;
const char* password = wife_creds.password;
const char * hostName = "downstairs";
const char* http_username = "admin";
const char* http_password = "admin";

//http://downstairs.local/index
//http://downstairs.local/relay.html
void setup(){

  Serial.begin(115200);
  Serial.setDebugOutput(true);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(hostName);
  WiFi.begin(ssid, password);

  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("STA: Failed!\n");
    WiFi.disconnect(false);
    delay(1000);
    WiFi.begin(ssid, password);
  }
  
  Serial.print("Local IP: ");  
  Serial.println(WiFi.localIP());

  //Send OTA events to the browser
  ArduinoOTA.onStart([]() { events.send("Update Iniciado", "ota"); });
  ArduinoOTA.onEnd([]() { events.send("Update Finalizado", "ota"); });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    char p[32];
    sprintf(p, "Progresso: %u%%\n", (progress/(total/100)));
    events.send(p, "ota");
  });

  ArduinoOTA.onError([](ota_error_t error) {
    if(error == OTA_AUTH_ERROR) events.send("Auth Falhou", "ota");
    else if(error == OTA_BEGIN_ERROR) events.send("Início Falhou", "ota");
    else if(error == OTA_CONNECT_ERROR) events.send("Conexão Falhou", "ota");
    else if(error == OTA_RECEIVE_ERROR) events.send("Recibimento Falhou", "ota");
    else if(error == OTA_END_ERROR) events.send("Final Falhou", "ota");
  });

  ArduinoOTA.setHostname(hostName);
  ArduinoOTA.begin();

  MDNS.addService("http","tcp",80);

  SPIFFS.begin();

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  events.onConnect([](AsyncEventSourceClient *client){
    client->send("hello!",NULL,millis(),1000);
  });
  server.addHandler(&events);

  server.addHandler(new SPIFFSEditor(SPIFFS, http_username,http_password));
 
  // Bootstrap -------------------  
  server.on("/bootstrap/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/bootstrap/bootstrap.min.css", "text/css");
  });

  server.on("/jquery/jquery.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/jquery/jquery.min.js", "text/js");
  });

  server.on("/bootstrap/bootstrap.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/bootstrap/bootstrap.min.js", "text/js");
  });
  // Bootstrap End -----------------

  // Relay visuals -----------------
  server.on("relay_scripts.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "relay_scripts.js", "text/js");
  });

  // When a button is pressed
  // -------Relay One---------
  server.on("/relay.html/RELAY_1_ON", HTTP_GET, [](AsyncWebServerRequest *request){
    quadRelay.toggleRelay(1);       
    request->send(200);  
  });

  // -------Relay Two---------
  server.on("/relay.html/RELAY_2_ON", HTTP_GET, [](AsyncWebServerRequest *request){
    quadRelay.toggleRelay(2);       
    request->send(200);  
  });

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("relay.html");

  server.onNotFound([](AsyncWebServerRequest *request){
    Serial.printf("NOT_FOUND: ");
    if(request->method() == HTTP_GET)
      Serial.printf("GET");
    else if(request->method() == HTTP_POST)
      Serial.printf("POST");
    else if(request->method() == HTTP_DELETE)
      Serial.printf("DELETE");
    else if(request->method() == HTTP_PUT)
      Serial.printf("PUT");
    else if(request->method() == HTTP_PATCH)
      Serial.printf("PATCH");
    else if(request->method() == HTTP_HEAD)
      Serial.printf("HEAD");
    else if(request->method() == HTTP_OPTIONS)
      Serial.printf("OPTIONS");
    else
      Serial.printf("UNKNOWN");
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if(request->contentLength()){
      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for(i=0;i<headers;i++){
      AsyncWebHeader* h = request->getHeader(i);
      Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();
    for(i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()){
        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if(p->isPost()){
        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    request->send(404);
  });
  server.onFileUpload([](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
    if(!index)
      Serial.printf("UploadStart: %s\n", filename.c_str());
    Serial.printf("%s", (const char*)data);
    if(final)
      Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index+len);
  });
  server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if(!index)
      Serial.printf("BodyStart: %u\n", total);
    Serial.printf("%s", (const char*)data);
    if(index + len == total)
      Serial.printf("BodyEnd: %u\n", total);
  });
  server.begin();

  // Quad Relay  
  Wire.begin();
  if (quadRelay.begin()){
    Serial.println("Pronto para virar alguns interruptores.");
  }
  else
    Serial.println("Não foi possível comunicar com o Relay.");

}

void loop(){
  ArduinoOTA.handle();
}
