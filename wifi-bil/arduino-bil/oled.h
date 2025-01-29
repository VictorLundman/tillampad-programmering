#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <U8x8lib.h>

void setupOled(U8X8_SSD1306_128X64_NONAME_SW_I2C* u8x8);
void oledWriteIp(U8X8_SSD1306_128X64_NONAME_SW_I2C* u8x8, String ip);
void oledWriteLoading(U8X8_SSD1306_128X64_NONAME_SW_I2C* u8x8);