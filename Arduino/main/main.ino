#include "drivers.h"
#include "movement.h"

void setup() {
  init_motor_pins();
  // init_kicker_pin();
  init_detect_pins();
  Serial.begin(9600);
  Serial.println("Start");
}

void loop() {
  int direction = detect_ball();
  Serial.print(direction);
  if (direction != -1) {
    move(direction); 
  } else {
    motors(0, 0, 0);
  }
  delay(100);
}
