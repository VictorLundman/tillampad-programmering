#include <Wire.h>
#include <SPI.h>
#include <U8x8lib.h>
#include <WiFiNINA.h>
#include "wifiPass.h"
#include "motors.h"
#include "oled.h"

/** Light for the status LED */
#define STATUS_PIN 7

/** Loads wifi configuration from header file (that is excluded from git) */
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

/** Init the server */
WiFiServer server(80);
int status = WL_IDLE_STATUS;

/** Init oled */
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display

/** Motos configuration */
bool leftOn = false;
bool rightOn = false;
int requests = 0;

void setup() {
  Serial.begin(9600);
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // Setup
  setupMotors();

  setupStatusLed();
  setStatusLed(HIGH);

  setupOled(&u8x8);
  oledWriteLoading(&u8x8);

  connectToWifi();
  printWifiStatus();


  setStatusLed(LOW);

  startServer();
}

void loop() {
  // Write info to led for debuging
  u8x8.drawString(0, 6, String(millis()).c_str());
  u8x8.drawString(0, 7, String(requests).c_str());

  WiFiClient client = server.available();
  if (client) {
    requests++;
    setStatusLed(HIGH);
    Serial.println("new client");
    boolean currentLineIsBlank = true;
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' && currentLineIsBlank) {
          // send a standard HTTP response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Access-Control-Allow-Origin: *");
          client.println("Access-Control-Allow-Methods: *");
          client.println("Access-Control-Allow-Headers: *");
          client.println("Access-Control-Max-Age: 86400");
          client.println("");
          client.print("{\"status\": \"success\", \"left\": ");
          client.print(leftOn);
          client.print(", \"right\": ");
          client.print(rightOn);
          client.println("}");
          break;
        }
        if (c == '\n') {
          // Handle data for the motos
          if (currentLine.startsWith("x-motor-data: ")) {
            leftOn = currentLine[14] == '1';
            rightOn = currentLine[15] == '1';

            Serial.println(currentLine);

            Serial.print("Set motors: ");
            Serial.print(leftOn);
            Serial.print(", ");
            Serial.println(rightOn);
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

    setMotors(leftOn, rightOn);

    delay(100);

    // close the connection:
    client.stop();

    // delay(100);

    setStatusLed(LOW);
    Serial.println("client disconnected");
  }
}

/** 
 * Connect to the wifi. 
 * Parameters: Void
 * Returns: Void
 */
void connectToWifi() {
  Serial.print("Connecting to Wifi (");
  Serial.print(ssid);
  Serial.print(", ");
  Serial.print(pass);
  Serial.print(")");
  int status;

  // Set timeout to 5 seconds
  WiFi.setTimeout(5 * 1000);
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);

    // Wait 5 seconds for connection:
    delay(5000);
  }

  Serial.println("");
  Serial.println("Connected!");
}

/** 
 * Convert ip adress to string. 
 * Parameters: IPAddress* address
 * Returns: String
 */
String toString(const IPAddress* address){
  return String() + (*address)[0] + "." + (*address)[1] + "." + (*address)[2] + "." + (*address)[3];
}

/** 
 * Write wifi status to serial and oled. . 
 * Parameters: Screen
 * Returns: Void
 */
void printWifiStatus() {
  Serial.print("Local ip: ");
  Serial.println(WiFi.localIP());

  oledWriteIp(&u8x8, toString(&WiFi.localIP()));
}

/** 
 * Start the web server. Run after connecting to wifi. 
 * Parameters: Void
 * Returns: Void
 */
void startServer() {
  Serial.println("Starting server");
  server.begin();
}

/** 
 * Initialize the status light. 
 * Parameters: Void
 * Returns: Void
 */
void setupStatusLed() {
  pinMode(STATUS_PIN, OUTPUT);
}

/** 
 * Set the oled. 
 * Parameters: bool status
 * Returns: Void
 */
void setStatusLed(bool status) {
  digitalWrite(STATUS_PIN, status);
}