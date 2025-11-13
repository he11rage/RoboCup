#define MOTOR_COUNT 3
#define KICKER 46
#define DT_OUTPUT_COUNT 4
#define DT_INPUT_PIN 10

const float POWER_COEF = 2.55f;  // Коэффициент для перевода от 0-100 к 0-255
const int POWER_FREE = 0;
const int POWER_MAX = 100;

const int DT_OUTPUT_PINS[DT_OUTPUT_COUNT] = { 2, 3, 4, 6 };
const int FI_pins[MOTOR_COUNT] = { 3, 7, 11 };  // Forward input pins
const int BI_pins[MOTOR_COUNT] = { 5, 9, 13 };  // Backward input pins

void motor(int power, int FI, int BI) {
  // Управление одним движком
  int backward_power = 0;
  int forward_power = 0;

  power = constrain(power, -POWER_MAX, POWER_MAX);

  if (power > POWER_FREE) {
    // Вперед
    forward_power = (int)(power * POWER_COEF);
    analogWrite(FI, forward_power);
    digitalWrite(BI, LOW);
  } else if (power < POWER_FREE) {
    // Назад
    backward_power = (int)((-power) * POWER_COEF);
    digitalWrite(FI, LOW);
    analogWrite(BI, backward_power);
  } else {
    digitalWrite(FI, LOW);
    digitalWrite(BI, LOW);
  }
}

void motors(int powers[MOTOR_COUNT]) {
  for (int i = 0; i < MOTOR_COUNT; i++) {
    motor(powers[i], FI_pins[i], BI_pins[i]);
  }
}
// обертка
void motors(int p0, int p1) {
  int powers[MOTOR_COUNT] = { 0 };
  if (MOTOR_COUNT > 0) powers[0] = p0;
  if (MOTOR_COUNT > 1) powers[1] = p1;
  motors(powers);
}

void init_motor_pins() {
  for (int i = 0; i < MOTOR_COUNT; i++) {
    pinMode(BI_pins[i], OUTPUT);
    pinMode(FI_pins[i], OUTPUT);
  }
}

void init_kicker_pin() {
  pinMode(KICKER, OUTPUT);
}

void kick() {
  Serial.println("Kick ON!");
  digitalWrite(KICKER, HIGH);  // Включаем реле
  delay(100);                  // 0.1 сек удержание
  digitalWrite(KICKER, LOW);   // Отключаем реле
  Serial.println("Kick OFF!");
}

void init_detect_pins() {
  for (int i = 0; i < DT_OUTPUT_COUNT; i++) {
    pinMode(DT_OUTPUT_PINS[i], OUTPUT);
  }
  pinMode(DT_INPUT_PIN, INPUT);
}

void setMuxChannel(byte channel) {
  for (int i = 0; i < DT_OUTPUT_COUNT; i++) {
    digitalWrite(DT_OUTPUT_PINS[i], (channel >> i) & 1);
  }
}
void detect_ball() {
  int active_count = 0;  // счетчик активных (сработавших) датчиков
  int values[16];        // буфер для 16 измерений с мультиплексора

  // Поочередно выбираем каждый канал мультиплексора и читаем аналоговый вход
  for (byte i = 0; i < 16; i++) {
    setMuxChannel(i);
    delay(2);                               // небольшая задержка для установления сигнала
    values[i] = digitalRead(DT_INPUT_PIN);  // фиксируем уровень от датчика i (LOW/HIGH)
  }

  // Для цифрового входа предполагаем активный низкий уровень (LOW)
  int min_index = -1, max_index = -1;  // крайние индексы сработавших датчиков (первый и последний)
  float sum_cos = 0.0f;                // накопитель X‑составляющей для кругового среднего
  float sum_sin = 0.0f;                // накопитель Y‑составляющей для кругового среднего

  // Проходим по всем сенсорам: определяем активность и копим векторную сумму направлений
  for (int i = 0; i < 16; i++) {
    int is_active = (values[i] == LOW) ? 1 : 0;  // активен при LOW
    if (is_active == 1) {
      active_count++;
      if (min_index == -1) {
        min_index = i;
      }
      max_index = i;

      // азимут датчика i на окружности (0..2π), затем добавляем его единичный вектор
      float angle_rad_i = (2.0f * PI) * (float)i / 16.0f;
      sum_cos += cos(angle_rad_i);
      sum_sin += sin(angle_rad_i);
    }
  }

  if (min_index != -1 && max_index != -1) {
    Serial.print("Ball between sensors ");
    Serial.print(min_index);
    Serial.print(" and ");
    Serial.println(max_index);
  } else {
    Serial.println("Ball not detected");
  }



  // Определение угла до мяча по активным датчикам (круговое среднее направления)
  if (active_count > 0) {  // продолжаем только если есть хотя бы один активный датчик
    // atan2 принимает (Y, X) — здесь (sum_sin, sum_cos), получаем результирующий угол в радианах
    float angle_rad = atan2(sum_sin, sum_cos);
    float angle_deg = angle_rad * 180.0f / PI;  // переводим угол из радиан в градусы
    if (angle_deg < 0.0f) {                     // нормализация угла к диапазону [0; 360)
      angle_deg += 360.0f;                      // преобразуем отрицательный угол в эквивалентный положительный
    }

    Serial.print("Angle to ball: ");
    Serial.print(angle_deg);
    Serial.println(" deg");
  }
}


// // Жесткое торможение (BRAKE): оба входа HIGH (короткое замыкание на мотор)
// void motorBrake() {
//   digitalWrite(FI, HIGH);
//   digitalWrite(BI, HIGH);
// }