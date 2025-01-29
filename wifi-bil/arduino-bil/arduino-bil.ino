#include <Wire.h>
#include <SPI.h>
#include <U8x8lib.h>
#include <WiFiNINA.h>
#include "wifiPass.h"
#include "motors.h"
#include "oled.h"


#define STATUS_PIN 7
// #define DISPLAY_DIN_PIN 12
// #define DISPLAY_CS_PIN 10
// #define DISPLAY_CLK_PIN 13

char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

WiFiServer server(80);
int status = WL_IDLE_STATUS;

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display

bool leftOn = false;
bool rightOn = false;

void setup() {
  Serial.begin(9600);
  while (!Serial) {};
  Serial.println("Here!");
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  setupMotors();
  setupStatusLed();
  // setupDisplay();

  setStatusLed(HIGH);
  setupOled(&u8x8);
  oledWriteLoading(&u8x8);

  connectToWifi();
  printWifiStatus();

  setStatusLed(LOW);

  startServer();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    setStatusLed(HIGH);
    Serial.println("new client");
    boolean currentLineIsBlank = true;
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the HTTP request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard HTTP response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Access-Control-Allow-Origin: *");
          client.println("Access-Control-Allow-Methods: *");
          client.println("Access-Control-Allow-Headers: *");
          client.println("Access-Control-Max-Age: 86400");
          // if (isPreflight) {
          //   client.println("Connection: keep-alive");
          // } else {
          //   client.println("Connection: close");
          // }

          client.println("");
          client.print("{\"status\": \"success\", \"left\": ");
          client.print(leftOn);
          client.print(", \"right\": ");
          client.print(rightOn);
          client.println("}");
          break;
        }
        if (c == '\n') {
          if (currentLine.startsWith("x-motor-data: ")) {
            leftOn = currentLine[14] == '1';
            rightOn = currentLine[15] == '1';

            Serial.println(currentLine);

            Serial.print("Set motors: ");
            Serial.print(leftOn);
            Serial.print(", ");
            Serial.println(rightOn);

            setMotors(leftOn, rightOn);
          } 

          // you're starting a new line
          currentLine = "";
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
          currentLine += c;
        }
      }
    }

    delay(100);

    // close the connection:
    client.stop();
    setStatusLed(LOW);
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
  WiFi.setTimeout(10 * 1000);
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(3000);
  }

  Serial.println("");
  Serial.println("Connected!");
}

String toString(const IPAddress* address){
  return String() + (*address)[0] + "." + (*address)[1] + "." + (*address)[2] + "." + (*address)[3];
}


void printWifiStatus() {
  Serial.print("Local ip: ");
  Serial.println(WiFi.localIP());

  oledWriteIp(&u8x8, toString(&WiFi.localIP()));

  // max7219.Clear();
}

void startServer() {
  Serial.println("Starting server");
  server.begin();
}

void setupStatusLed() {
  pinMode(STATUS_PIN, OUTPUT);
}

void setStatusLed(bool status) {
  digitalWrite(STATUS_PIN, status);
}