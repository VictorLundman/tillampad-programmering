/*
* Name: clock and temp project
* Author: Victor Lundman
* Date: 2024-10-25
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display,
* Further, it measures temprature with a analog temprature module and displays a mapped value to a 9g-servo-motor
*/

// Include Libraries
#include <RTClib.h>
#include <Wire.h>
#include <U8glib.h>
#include <Servo.h>

// Init constants
#define SERVO_PIN 9
#define TEMP_SENS_PIN A0
#define BUTTON_PIN 8

// Init global variables
float R1 = 10000;
float logR2, R2, T, Tc, Tf;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

// construct objects
RTC_DS3231 rtc;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
Servo servo;

void setup() {
  // init communication
  Serial.begin(9600);
  Wire.begin();

  // Init Hardware
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  servo.attach(SERVO_PIN);

  pinMode(TEMP_SENS_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);

  // Other
  u8g.setFont(u8g_font_unifont);
}

void loop() {
  // Only start the game if the button is down. The function returns early if the game is already running, so no need to handle it here, same for the updateGameState function.
  if (isButtonDown()) startGame();
  updateGameState();

  // Oled write only writes the time if the game is not currently running. 
  oledWrite(getTime());
  servoWrite(getTemp());

  delayGameFrame();
}


/*
*This function reads time from an ds3231 module and package the time as a String
*Parameters: Void
*Returns: time in hh:mm:ss format as String
*/
String getTime() {
  DateTime now = rtc.now();
  return String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
}

/*
* This function reads an analog pin connected to an analog temprature sensor and calculates the corresponding temp
* Parameters: Void
* Returns: temprature in celcius as float
*/
float getTemp() {
  const int analogTemp = analogRead(TEMP_SENS_PIN);

  // Calculate temp in Celcius (https://www.circuitbasics.com/arduino-thermistor-temperature-sensor-tutorial/)
  R2 = R1 * (1023.0 / (float)analogTemp - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  Tc = T - 273.15;

  return Tc;
}

/*
 * Get the button state on the `BUTTON_PIN` button. 
 * Parameters: None
 * Returns bool - true if button is down.
 */
bool isButtonDown() {
  return digitalRead(BUTTON_PIN);
}

// The Flappy Bird game state. 
bool gameActive = false;
bool lostLastGame = false;
unsigned int score = 0;
const int birdX = 24;
int birdY = 32;
/// 2D array (x, Y)
int pipes[2][2] = {{-1, 0}, {-1, 0}};
int pipeOpeningHeight = 10;

/*
 * Delays the process/thread. 
 * Parameters: None
 * Returns: Void
 */
void delayGameFrame() {
  if (gameActive) delay(25);
  else delay(100);
}

/*
 * Updates the game state. 
 * Parameters: None
 * Returns: Void
 */
void updateGameState() {
  if (!gameActive) return;

  // For every pipe. 
  for (int i = 0; i < 2; i++) {
    // Skip if pipe is not initialized. 
    if (pipes[i][0] == -1) continue;
    pipes[i][0] -= 2;

    bool birdPassedPipe = pipes[i][0] <= birdX && (pipes[i][0] + 2) > birdX;
 
    // If lost game. 
    if (birdPassedPipe && 
      (birdY < pipes[i][1] || 
      birdY > (pipes[i][1] + pipeOpeningHeight))
    ) {
      gameActive = false;
      lostLastGame = true;

      delay(1000);
      return;
    }

    if (birdPassedPipe) {
      score++;
    }
  }

  // If pipe one is out of bounds, make pipe 2 pipe one and deinit the out of bounds one. 
  if (pipes[0][0] < 0) {
    pipes[0][0] = pipes[1][0];
    pipes[0][1] = pipes[1][1];
    pipes[1][0] = -1;
    pipes[1][1] = 0;
  }

  // Spawn the new pipe when the old one is halfway across the screen. 
  bool shoudSpawnNewPipe = pipes[0][0] < 64;
  // Make sure at least one pipe is currently inactive. 
  if (shoudSpawnNewPipe && (pipes[1][0] == -1 || pipes[0][0] == -1)) {
    int index = pipes[0][0] == -1 ? 0 : 1;
    pipes[index][0] = 128;
    pipes[index][1] = random(20, 50);
  }

  // Only move bird up if button is down. Else move it down. 
  if (isButtonDown()) birdY -= 1;
  else birdY += 1;

  // If bird os out of bounds, kill it. 
  if (birdY < 0 || birdY > 64) {
    gameActive = false;
    lostLastGame = true;

    // Delay the game for the user to know why they died. 
    delay(1000);
    return;
  }
}

/*
 * Updates the game state to start the game next update call. 
 * Parameters: None
 * Returns None
 */
void startGame() {
  // Skip if game already is active. 
  if (gameActive) return;

  gameActive = true;
  score = 0;
  birdY = 32;
  pipes[0][0] = -1;
  pipes[0][1] = 0;
  pipes[1][0] = -1;
  pipes[1][1] = 0;
}

/*
* This function takes a string and draws it to an oled display
*Parameters: - text: String to write to display
*Returns: void
*/
void oledWrite(String text) {
  u8g.firstPage();
  do {
    // Show default screen if game is inactive. 
    if (!gameActive) {
      u8g.drawStr(0, 22, text.c_str());

      // The player might have lost the last game, so we tell them it here. 
      if (lostLastGame) {
        u8g.drawStr(0, 35, "You lost!");
        u8g.drawStr(0, 48, String(score).c_str());
      }

      // Don't draw the game, continue. 
      continue;
    } 

    // Draw bird. 
    u8g.drawBox(birdX, birdY, 1, 1);

    // Draw pipes. 
    for (int i = 0; i < 2; i++) {
      // Top pipe
      u8g.drawBox(pipes[i][0], 0, 1, pipes[i][1]);
      // Bottom pipe
      u8g.drawBox(pipes[i][0], pipes[i][1] + pipeOpeningHeight, 1, 128);
    }

    // Draw score. 
    u8g.drawStr(0, 10, String(score).c_str());
  } while (u8g.nextPage());
}

/*
* takes a temprature value and maps it to corresppnding degree on a servo
*Parameters: - value: temprature
*Returns: void
*/
void servoWrite(float value) {
  const int deg = map(value, 0, 40, 0, 180);
  servo.write(deg);
}