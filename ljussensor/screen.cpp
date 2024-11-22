#include "screen.h"
#include <Arduino.h>

void writeToScreen(Screen* screen, GraphDataArray* data, int minVal, int maxVal) {
  screen->firstPage();
  do {
    for (int i = 1; i < GRAPH_DATA_LENGTH; i++) {
      const int yFrom = OLED_HEIGHT - map((*data)[i - 1], minVal, maxVal, 0, OLED_HEIGHT);
      const int yTo = OLED_HEIGHT - map((*data)[i], minVal, maxVal, 0, OLED_HEIGHT);
      screen->drawLine(OLED_WIDTH - i, yTo, OLED_WIDTH - i + 1, yFrom);
    }
  } while (screen->nextPage());
}