#include <U8glib.h>

#define SENSOR_PIN A0
#define SENSOR_DELAY_MS 100
#define OLED_WIDTH 127
#define OLED_HEIGHT 63

#define AVERAGE_READINGS_LENGTH 128
float averageReadings[AVERAGE_READINGS_LENGTH];

#define LAST_READINGS_LENGTH 10
unsigned int lastReadings[LAST_READINGS_LENGTH];

unsigned int index = 0;

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
#include <U8glib.h>

void setup() {
  pinMode(SENSOR_PIN, INPUT);

  Serial.begin(9600);
}

void loop() {
  const int average = getAverage();

  for (int i = 1; i < AVERAGE_READINGS_LENGTH; i++) {
    averageReadings[i - 1] = averageReadings[i];
  }

  averageReadings[AVERAGE_READINGS_LENGTH - 1] = average;

  int minVal = 1027;
  int maxVal = 0;
  for (int i = 0; i < AVERAGE_READINGS_LENGTH; i++) {
    const int val = averageReadings[i];
    if (val < minVal) {
      minVal = val;
    } else if (val > maxVal) {
      maxVal = val;
    }
  }

  writeToScreen(minVal, maxVal);
}

int getAverage() {
  const unsigned int reading = analogRead(SENSOR_PIN);
  lastReadings[index] = reading;

  index++;
  if (index == LAST_READINGS_LENGTH) index = 0;

  unsigned int sum = 0;
  for (int i = 0; i < LAST_READINGS_LENGTH; i++) {
    sum += lastReadings[i];
  }

  const int average = sum / LAST_READINGS_LENGTH;

  // Serial.print(reading);
  // Serial.print(", ");
  // Serial.println(average);

  // delay(SENSOR_DELAY_MS);

  return average;
}

void writeToScreen(int minVal, int maxVal) {
  u8g.firstPage();
  do {
    for (int i = 1; i < AVERAGE_READINGS_LENGTH; i++) {
      const int yFrom = OLED_HEIGHT - map(averageReadings[i - 1], minVal, maxVal, 0, OLED_HEIGHT);
      const int yTo = OLED_HEIGHT - map(averageReadings[i], minVal, maxVal, 0, OLED_HEIGHT);
      // Serial.print(yFrom);
      // Serial.println(averageReadings[i - 1]);
      u8g.drawLine(OLED_WIDTH - i, yTo, OLED_WIDTH - i + 1, yFrom);
    }
  } while (u8g.nextPage());
}