#include <U8glib.h>
#include "average.h"
#include "graphData.h"
#include "screen.h"

#define SENSOR_PIN A0
#define SENSOR_DELAY_MS 100

GraphDataArray graphData;

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);

void setup() {
  pinMode(SENSOR_PIN, INPUT);

  Serial.begin(9600);
}

void loop() {
  const int reading = analogRead(SENSOR_PIN);
  const int average = advanceGetAverage(reading);
  advanceGraphData(&graphData, average);
  MinMax minMax = getDataMinMax(&graphData);

  writeToScreen(&u8g, &graphData, minMax.min, minMax.max);
}