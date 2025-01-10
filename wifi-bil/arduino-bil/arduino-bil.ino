#include <WiFiNINA.h>
#include <WebSocketsServer.h>
#include "wifiPass.h"

char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

unsigned int connectedCount = 0;

WiFiServer server(80);
int status = WL_IDLE_STATUS;

bool leftOn = false;
bool rightOn = false;

void setup() {
  Serial.begin(9600);
  while (!Serial) {};
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  connectToWifi();
  printWifiStatus();

  startServer();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    boolean currentLineIsBlank = true;
    String currentLine = "";                
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the HTTP request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard HTTP response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Access-Control-Allow-Origin: *");
          client.println("Connection: close");
          client.println("");
          client.print("{status: \"success\", \"left\": ");
          client.print(leftOn);
          client.print(", \"right\": ");
          client.print(rightOn);
          client.println("}");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLine = "";
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
          currentLine += c; 
        }

        if (currentLine.startsWith("data: ")) {
          leftOn = currentLine[6] == "1";
          rightOn = currentLine[7] == "1";

          Serial.print("Set motors: ");
          Serial.print(leftOn);
          Serial.print(", ");
          Serial.println(rightOn);
        }
      }
    }

    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void connectToWifi() {
  Serial.print("Connecting to Wifi (");
  Serial.print(ssid);
  Serial.print(", ");
  Serial.print(pass);
  Serial.print(")");
  int status;
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.println("");
  Serial.println("Connected!");
}

void printWifiStatus() {
  Serial.print("Local ip: ");
  Serial.println(WiFi.localIP());
}

void startServer() {
  Serial.println("Starting server");
  server.begin();
}