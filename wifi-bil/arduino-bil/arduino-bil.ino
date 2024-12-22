#include <WiFiNINA.h>
#include <WebSocketsServer.h>
#include "wifiPass.h"

char ssid[] = WIFI_SSID; 
char pass[] = WIFI_PASS;

unsigned int connectedCount = 0;

WebSocketsServer webSocket = WebSocketsServer(80);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  connectToWifi();
  printWifiStatus();

  startWebSocketServer();
}

void loop() {
  webSocket.loop();
}

void connectToWifi() {
  Serial.print("Connecting to Wifi");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected!");
}

void printWifiStatus() {
  Serial.print("Local ip: ");
  Serial.println(WiFi.localIP());
}

void startWebSocketServer() {
  webSocket.begin();
  webSocket.onEvent(handleWebSocketEvent);
}

void handleWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      connectedCount--;

      Serial.println("Client disconnected!");
      break;

    case WStype_CONNECTED:
      connectedCount++;
      
      Serial.println("Client connected!");
      break; 

    case WStype_BIN:
      Serial.print("[WSc] get binary length: ");
      Serial.println(length);

      webSocket.sendBIN([1], 1);

      break;

    default:
      break;
  }
}