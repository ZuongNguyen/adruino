// Mạch thử nghiệm 1 servo qua mạch PCA9685 (kênh 0) – Arduino Uno
// Mạch:
//   PCA9685 GND -> GND Arduino (dây đen)
//   PCA9685 VCC -> 5V Arduino  (dây đỏ, chỉ nuôi chip PCA9685, không nuôi servo)
//   PCA9685 SDA -> A4 (dây xanh)
//   PCA9685 SCL -> A5 (dây vàng)
//   Servo cắm cột 0 của PCA9685: dây cam -> PWM, dây đỏ -> V+, dây nâu -> GND
//   Cầu đấu xanh trên PCA9685 -> nguồn riêng cho servo, 5V, TỐI ĐA 6V (xem README)
//
// Cần thư viện "Adafruit PWM Servo Driver Library" (Library Manager, cài kèm Adafruit BusIO).
//
// Hiệu ứng: quét đều 0° <-> 180° QUET_ROUNDS vòng, rồi đi từng nấc 0° -> 45° -> ... -> 180°
// và đi từng nấc về 0° NAC_ROUNDS vòng, lặp lại. Vòng nào cũng bắt đầu và kết thúc ở 0°.
// Khi khởi động không tìm thấy PCA9685 thì đèn L trên Arduino nháy nhanh.

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

const uint8_t PCA_ADDRESS    = 0x40;          // địa chỉ I2C mặc định (chưa hàn cầu A0–A5 trên mạch)
const uint8_t SERVO_CHANNEL  = 0;             // kênh 0–15 của PCA9685, không phải chân Arduino
const uint8_t ERROR_LED_PIN  = 13;            // đèn L trên Arduino, nháy khi không thấy PCA9685

const int MAX_ANGLE = 180;

// ---- Thông số servo ----
const unsigned long SERVO_MIN_US = 544;       // độ rộng xung ở 0°, giống mặc định của Servo.h
const unsigned long SERVO_MAX_US = 2400;      // độ rộng xung ở 180°
const unsigned long PWM_HZ       = 50;        // servo dùng xung 50 Hz
const unsigned long PCA_OSC_HZ   = 27000000;  // tần số dao động thực tế của PCA9685 theo Adafruit
const unsigned long PCA_STEPS    = 4096;      // PCA9685 chia mỗi chu kỳ thành 4096 bước

// ---- Thông số hiệu ứng (đơn vị: mili giây) ----
const unsigned long QUET_MOVE_MS = 2700;      // quay hết 180°
const unsigned long QUET_HOLD_MS = 500;       // dừng ở mỗi đầu trước khi quay ngược
const unsigned long QUET_ROUNDS  = 2;         // số vòng quét, đặt 0 để bỏ

const unsigned long NAC_COUNT    = 4;         // số nấc mỗi lượt (4 nấc = mỗi nấc 45°)
const unsigned long NAC_MOVE_MS  = 400;       // thời gian quay hết một nấc
const unsigned long NAC_HOLD_MS  = 600;       // dừng ở mỗi nấc
const unsigned long NAC_ROUNDS   = 1;         // số vòng đi từng nấc, đặt 0 để bỏ

const unsigned long ERROR_BLINK_MS = 100;     // nhịp nháy đèn L khi không thấy PCA9685

// Một vòng gồm lượt lên và lượt về, mỗi lượt đã có khoảng dừng ở cuối
const unsigned long QUET_HALF_MS  = QUET_MOVE_MS + QUET_HOLD_MS;
const unsigned long QUET_ROUND_MS = 2 * QUET_HALF_MS;
const unsigned long QUET_TOTAL_MS = QUET_ROUNDS * QUET_ROUND_MS;

const unsigned long NAC_STEP_MS  = NAC_MOVE_MS + NAC_HOLD_MS;
const unsigned long NAC_HALF_MS  = NAC_COUNT * NAC_STEP_MS;
const unsigned long NAC_ROUND_MS = 2 * NAC_HALF_MS;
const unsigned long NAC_TOTAL_MS = NAC_ROUNDS * NAC_ROUND_MS;

const unsigned long SHOW_MS = QUET_TOTAL_MS + NAC_TOTAL_MS;

Adafruit_PWMServoDriver pca(PCA_ADDRESS);
bool pcaOk = false;
int lastAngle = 0;                            // góc đã gửi, chỉ gửi lại khi thay đổi

// Đổi góc thành số bước bật xung của PCA9685 trong một chu kỳ
uint16_t pulseTicks(int angle) {
  unsigned long us = SERVO_MIN_US + (SERVO_MAX_US - SERVO_MIN_US) * (unsigned long)angle / MAX_ANGLE;
  return us * PCA_STEPS * PWM_HZ / 1000000UL;
}

// Góc tăng đều từ 0 tới span trong khoảng duration, tính tại thời điểm t
int rampAngle(unsigned long t, unsigned long duration, int span) {
  if (duration == 0 || t >= duration) return span;
  return (long)span * t / duration;
}

// Hiệu ứng quét: góc tại thời điểm t trong một vòng
int angleQuet(unsigned long t) {
  if (t < QUET_HALF_MS) return rampAngle(t, QUET_MOVE_MS, MAX_ANGLE);
  return MAX_ANGLE - rampAngle(t - QUET_HALF_MS, QUET_MOVE_MS, MAX_ANGLE);
}

// Lượt đi lên từng nấc: nấc n đi từ góc `from` tới góc `to` rồi dừng
int nacUp(unsigned long t) {
  unsigned long n = t / NAC_STEP_MS;
  int from = MAX_ANGLE * n / NAC_COUNT;
  int to   = MAX_ANGLE * (n + 1) / NAC_COUNT;
  return from + rampAngle(t % NAC_STEP_MS, NAC_MOVE_MS, to - from);
}

// Hiệu ứng từng nấc: lượt về đối xứng với lượt lên
int angleNac(unsigned long t) {
  if (t < NAC_HALF_MS) return nacUp(t);
  return MAX_ANGLE - nacUp(t - NAC_HALF_MS);
}

// Góc tại thời điểm t trong cả chương trình
int programAngle(unsigned long t) {
  if (t < QUET_TOTAL_MS) return angleQuet(t % QUET_ROUND_MS);
  return angleNac((t - QUET_TOTAL_MS) % NAC_ROUND_MS);
}

void writeAngle(int angle) {
  pca.setPWM(SERVO_CHANNEL, 0, pulseTicks(angle));
}

void setup() {
  pinMode(ERROR_LED_PIN, OUTPUT);
  pcaOk = pca.begin();
  if (!pcaOk) return;
  pca.setOscillatorFrequency(PCA_OSC_HZ);     // phải đặt sau begin()
  pca.setPWMFreq(PWM_HZ);
  writeAngle(0);                              // đưa servo về 0° khi khởi động
  lastAngle = 0;
}

void loop() {
  if (!pcaOk) {
    digitalWrite(ERROR_LED_PIN, (millis() / ERROR_BLINK_MS) % 2);
    return;
  }
  if (SHOW_MS == 0) return;

  int angle = programAngle(millis() % SHOW_MS);
  if (angle != lastAngle) {
    writeAngle(angle);
    lastAngle = angle;
  }
}
