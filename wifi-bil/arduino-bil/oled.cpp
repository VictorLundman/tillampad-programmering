#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <U8x8lib.h>

/** 
 * Initialize the oled. 
 * Parameters: Screen
 * Returns: Void
 */
void setupOled(U8X8_SSD1306_128X64_NONAME_SW_I2C* u8x8) {
  u8x8->begin();
  // u8x8->setPowerSave(0);
  u8x8->setFont(u8x8_font_chroma48medium8_r);
}

/** 
 * Writes "loading" to the display. 
 * Parameters: Screen
 * Returns: Void
 */
void oledWriteLoading(U8X8_SSD1306_128X64_NONAME_SW_I2C* u8x8) {
  u8x8->drawString(0, 3, "Loading... ");
  u8x8->drawString(0, 4, "(slow)");
}

/** 
 * Writes the ip to the display. 
 * Parameters: Screen, String ip
 * Returns: Void
 */
void oledWriteIp(U8X8_SSD1306_128X64_NONAME_SW_I2C* u8x8, String ip) {
  // Serial.println(ip);
  u8x8->drawString(0, 3, ip.c_str());
}