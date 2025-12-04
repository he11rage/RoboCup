const float DELTA = 60.0f;
const float MOTOR_ANGLES[MOTOR_COUNT] = { DELTA + 90.0f, DELTA + 210.0f, DELTA + 330.0f };


int azimuth(float angle, int motor_number) {
  angle = -angle;
  float motor_angle = MOTOR_ANGLES[motor_number];
  float diff_angle = angle - motor_angle;
  float angle_rad = diff_angle * PI / 180.0f;
  float coef = cos(angle_rad);
  int power = (int)(POWER_MAX * coef);
  power = constrain(power, -POWER_MAX, POWER_MAX);

  return power;
}

void move(int angle) {
  angle = (float)(angle);
  motors(azimuth(angle, 0), azimuth(angle, 1), azimuth(angle, 2));
}