#include "drivers.h"


void setup() {
  // init_motor_pins();
  // init_kicker_pin();
  init_detect_pins();
  Serial.begin(9600);
  Serial.println("Start");
}


void loop() {
  // Serial.println("Another iteration");
  // motors(40,-40);
  // delay(5000);
  
  // Serial.println("Change direction");
  // motors(-40,40); 
  Serial.println("Detect ball");
  detect_ball();
  delay(1000);
}

