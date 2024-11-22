#include "screen.h"
#include <Arduino.h>

void advanceGraphData(GraphDataArray* data, int average) {
  for (int i = 1; i < GRAPH_DATA_LENGTH; i++) {
    // Serial.println((*data)[i]);
    (*data)[i - 1] = (*data)[i];
  }

  (*data)[GRAPH_DATA_LENGTH - 1] = average;
}


MinMax getDataMinMax(GraphDataArray* data) {
  int minVal = 1027;
  int maxVal = 0;

  for (int i = 0; i < GRAPH_DATA_LENGTH; i++) {
    const int val = (*data)[i];
    if (val < minVal) {
      minVal = val;
    }
    
    if (val > maxVal) {
      maxVal = val;
    }
  }

  MinMax minMax;
  minMax.min = minVal;
  minMax.max = maxVal;

  return minMax;
}