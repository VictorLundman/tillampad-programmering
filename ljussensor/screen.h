#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include <U8glib.h>
#include "graphData.h"

#define OLED_WIDTH 127
#define OLED_HEIGHT 63

typedef U8GLIB_SSD1306_128X64 Screen;

void writeToScreen(Screen* screen, GraphDataArray* data, int minVal, int maxVal);

#endif