#include <Arduino.h>

#define MOTOR_RELAY_L 13
#define MOTOR_RELAY_R 12

static void setRelay(unsigned int pin, bool mode) {
  digitalWrite(pin, mode);
}

void stopMotors() {
  setRelay(MOTOR_RELAY_L, LOW);
  setRelay(MOTOR_RELAY_R, LOW);
}

void runMotorsForward() {
  setRelay(MOTOR_RELAY_L, HIGH);
  setRelay(MOTOR_RELAY_R, HIGH);
}

void runMotorsLeft() {
  setRelay(MOTOR_RELAY_L, LOW);
  setRelay(MOTOR_RELAY_R, HIGH);
}

void runMotorsRight() {
  setRelay(MOTOR_RELAY_L, HIGH);
  setRelay(MOTOR_RELAY_R, LOW);
}