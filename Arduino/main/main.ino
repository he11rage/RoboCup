#include "drivers.h"
#include "movement.h"

void setup() {
  init_motor_pins();
  // init_kicker_pin();
  // init_detect_pins();
  Serial.begin(9600);
  Serial.println("Start");
}

void loop() {
  move(90);
  delay(5000);
  move(270);
  delay(5000);
  move(0);
  delay(5000);
  move(180);
  delay(5000);
  // Serial.println("Detect ball");
  // detect_ball();
  // delay(1000);
}
