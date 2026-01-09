#include "drivers.h"
#include "movement.h"

unsigned long lastUpdateTime = 0;
const unsigned long TIMEOUT_MS = 500;
int lastError = 0;

void setup() {
  init_motor_pins();
  // init_kicker_pin();
  init_detect_pins();
  Serial.begin(9600);
  Serial2.begin(9600);
  lastUpdateTime = millis();
}

int uart() {
  static String inputString = "";
  static bool newData = false;
  static int lastAngle = 999;

  while (Serial2.available()) {
    char inChar = (char)Serial2.read();
    if (inChar == '\n') {
      newData = true;
      break;
    }
    if (inChar == '\r') {
      continue;
    }
    inputString += inChar;
  }

  if (newData) {
    inputString.trim();
    if (inputString.length() > 0) {
      lastAngle = inputString.toInt();
      lastUpdateTime = millis();
    }
    inputString = "";
    newData = false;
  }

  if (millis() - lastUpdateTime > TIMEOUT_MS) {
    Serial.println("Timeout: Gates lost");
    return 999; 
  }

  Serial.print("Current angle from CV: ");
  Serial.println(lastAngle);
  return lastAngle;
}

int normalizeAngle(int angle) {
  int result = angle;
  if (result < 0) {
    result += 360;
  }
  return result;
}

int calculateRotation(int currentAngle, int targetAngle) {
  int error = currentAngle - targetAngle; 

  while (error <= -180) error += 360;
  while (error > 180) error -= 360;

  float Kp = 0.4;
  float Kd = 0.1;

  int derivative = error - lastError;
  lastError = error;

  int turnSpeed = (int)(error * Kp);

  int absError = abs(error);
  

  if (absError <= 15) return 0;

  return constrain(turnSpeed, -40, 40); 
}

void loop() {
  int ball_dir = detect_ball();

  int cv_error = uart();

  int target_angle = 0;
  int rotation_speed = 0;

  if (cv_error != 999) {
    rotation_speed = calculateRotation(cv_error, target_angle);
  }

  if (ball_dir != -1) {
    int ball_error = ball_dir;
    if (ball_error > 180) ball_error -= 360;

    float k_orbit = 2.0;

    int move_dir = (int)(ball_error * k_orbit);

    if (move_dir > 180) move_dir -= 360;
    if (move_dir < -180) move_dir += 360;

    // Serial.print("Ball: "); Serial.print(ball_error);
    // Serial.print(" | Move: "); Serial.println(move_dir);

    move(move_dir, rotation_speed);
  } else {
    move(-1, rotation_speed);
  }

  // static int mock_cv_error = 0;

  // if (Serial.available() > 0) {
  //   mock_cv_error = Serial.parseInt();
  //   Serial.print("Testing with angle:");
  //   Serial.println(mock_cv_error);
  // }

  // int target_angle = 0;
  // int rotation_speed = calculateRotation(mock_cv_error, target_angle);

  // move(-1, rotation_speed);

  delay(1);
}
