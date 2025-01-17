#include <WiFiNINA.h>
#include <max7219.h>
#include "wifiPass.h"
#include "motors.h"

#define STATUS_PIN 7
#define DISPLAY_DIN_PIN 12
#define DISPLAY_CS_PIN 11
#define DISPLAY_CLK_PIN 10

char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

WiFiServer server(80);
int status = WL_IDLE_STATUS;

MAX7219 max7219;

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

  max7219.Begin();

  max7219.Clear();

  auto ip = IPAddress(192, 168, 192, 216);

  unsigned n1 = ip[2];
  unsigned n2 = ip[3];

  Serial.println(n1);
  Serial.println(n2);

  Serial.print((n1 / 1U) & 10);
  Serial.print((n1 / 10U) & 10);
  Serial.print((n1 / 100U) & 10);
  Serial.print((n2 / 1U) & 10);
  Serial.print((n2 / 10U) & 10);
  Serial.println((n2 / 100U) & 10);

  max7219.DisplayChar(0, (ip[2] / 1U) & 1, false);
  max7219.DisplayChar(1, (ip[2] / 10U) & 10, false);
  max7219.DisplayChar(2, (ip[2] / 100U) & 100, true);

  max7219.DisplayChar(3, (ip[3] / 1U) & 1, false);
  max7219.DisplayChar(4, (ip[3] / 10U) & 10, false);
  max7219.DisplayChar(5, (ip[3] / 100U) & 100, false);

  setStatusLed(HIGH);

  connectToWifi();
  printWifiStatus();

  setStatusLed(LOW);

  startServer();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    boolean currentLineIsBlank = true;
    boolean isPreflight = false;
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
          if (!isPreflight) {
            client.print("{\"status\": \"success\", \"left\": ");
          }
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
          } else if (currentLine.startsWith("OPTIONS")) {
            isPreflight = true;
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

void printWifiStatus() {
  Serial.print("Local ip: ");
  Serial.println(WiFi.localIP());

  max7219.Clear();

  // max7219.DisplayChar(0, (ip[2] / 1U) & 1, false);
  // max7219.DisplayChar(1, (ip[2] / 10U) & 10, false);
  // max7219.DisplayChar(2, (ip[2] / 100U) & 100, true);

  // max7219.DisplayChar(3, (ip[3] / 1U) & 1, false);
  // max7219.DisplayChar(4, (ip[3] / 10U) & 10, false);
  // max7219.DisplayChar(5, (ip[3] / 100U) & 100, false);

  // IPAddress ip = WiFi.localIP();

  // // digitalWrite(DISPLAY_CS_PIN, HIGH);

  // Serial.println(ip[2]);

  // Serial.print((ip[2] / 1U) & 1);
  // Serial.print((ip[2] / 10U) & 10);
  // Serial.print((ip[2] / 100U) & 100);
  // Serial.print((ip[3] / 1U) & 1);
  // Serial.print((ip[3] / 10U) & 10);
  // Serial.println((ip[3] / 100U) & 100);

  // writeDisplay(8, (ip[2] / 1U) & 1);
  // writeDisplay(7, (ip[2] / 10U) & 10);
  // writeDisplay(6, (ip[2] / 100U) & 100);

  // writeDisplay(5, (ip[3] / 1U) & 1);
  // writeDisplay(4, (ip[3] / 10U) & 10);
  // writeDisplay(3, (ip[3] / 100U) & 100);
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

void setupDisplay() {
  digitalWrite(DISPLAY_CS_PIN, HIGH);
  pinMode(DISPLAY_DIN_PIN, OUTPUT);
  pinMode(DISPLAY_CS_PIN, OUTPUT);
  pinMode(DISPLAY_CLK_PIN, OUTPUT);

  writeDisplay(0x0f, 0x00);  //display test register - test mode off
  writeDisplay(0x0c, 0x01);  //shutdown register - normal operation
  writeDisplay(0x0b, 0x07);  //scan limit register - display digits 0 thru 7
  writeDisplay(0x0a, 0x0f);  //intensity register - max brightness
  writeDisplay(0x09, 0xff);  //decode mode register - CodeB decode all digits
  writeDisplay(0x08, 0x0c);  //digit 7 (leftmost digit) data
  writeDisplay(0x07, 0x0b);
  writeDisplay(0x06, 0x0d);
  writeDisplay(0x05, 0x0e);
  writeDisplay(0x04, 0x08);
  writeDisplay(0x03, 0x07);
  writeDisplay(0x02, 0x06);
  writeDisplay(0x01, 0x05);  //digit 0 (rightmost digit) data
}

void writeDisplay(byte address, byte data) {
  digitalWrite(DISPLAY_CS_PIN, LOW);
  shiftOut(DISPLAY_DIN_PIN, DISPLAY_CLK_PIN, MSBFIRST, address);
  shiftOut(DISPLAY_DIN_PIN, DISPLAY_CLK_PIN, MSBFIRST, data);
  digitalWrite(DISPLAY_CS_PIN, HIGH);
}