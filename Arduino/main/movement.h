const float DELTA = 60.0f;
const float MOTOR_ANGLES[MOTOR_COUNT] = { DELTA + 110.0f, DELTA + 230.0f, DELTA + 350.0f };


int azimuth(float angle, int motor_number) {
  angle = -angle;
  float motor_angle = MOTOR_ANGLES[motor_number];
  float diff_angle = angle - motor_angle;
  float angle_rad = diff_angle * PI / 180.0f;
  float coef = cos(angle_rad);
  int power = (int)(50 * coef);
  power = constrain(power, -50, 50);
  // Serial.println("");
  // Serial.print(motor_number);
  // Serial.print(" motor power is: ");
  // Serial.print(power);
  // Serial.println("");
  return power;
}

void move(int angle, int rotation = 0) {
  int p0, p1, p2;

  if (angle == -1) {
    p0 = rotation;
    p1 = rotation;
    p2 = rotation;
  } else {
    float f_angle = (float)(angle);
    p0 = azimuth(f_angle, 0) + rotation;
    p1 = azimuth(f_angle, 1) + rotation;
    p2 = azimuth(f_angle, 2) + rotation;
  }

  motors(p0, p1, p2);
}