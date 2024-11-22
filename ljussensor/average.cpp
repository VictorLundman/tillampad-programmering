#include "average.h"

#define LAST_READINGS_LENGTH 10
int lastReadings[LAST_READINGS_LENGTH];
unsigned int index = 0;

int advanceGetAverage(int reading) {
  lastReadings[index] = reading;

  index++;
  if (index == LAST_READINGS_LENGTH) index = 0;

  unsigned int sum = 0;
  for (int i = 0; i < LAST_READINGS_LENGTH; i++) {
    sum += lastReadings[i];
  }

  const int average = sum / LAST_READINGS_LENGTH;

  return average;
}