#include <Arduino.h>

#define MOTOR_RELAY_L 6
#define MOTOR_RELAY_R 5

static void setRelay(unsigned int pin, bool mode) {
  digitalWrite(pin, mode);
}

/** 
 * Initialize the motors. 
 * Parameters: Void
 * Returns: Void
 */
void setupMotors() {
  pinMode(MOTOR_RELAY_L, OUTPUT);
  pinMode(MOTOR_RELAY_R, OUTPUT);
}

/** 
 * Set the motors. 
 * Parameters: bool left, bool right
 * Returns: Void
 */
void setMotors(bool left, bool right) {
  setRelay(MOTOR_RELAY_L, left);
  setRelay(MOTOR_RELAY_R, right);
}

