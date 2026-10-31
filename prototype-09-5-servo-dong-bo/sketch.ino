// 5 servo quay đồng bộ cùng lúc 0° <-> 180° – Arduino Uno
// Mạch (servo xếp từ trái sang phải phía trên Arduino, dây xanh = tín hiệu):
//   Servo 1 -> chân 11   Servo 2 -> chân 9   Servo 3 -> chân 6
//   Servo 4 -> chân 5    Servo 5 -> chân 3
//   Dây đỏ của 5 servo -> hàng nguồn + trên breadboard (nối 5V Arduino)
//   Dây nâu của 5 servo -> hàng nguồn - trên breadboard (nối GND Arduino)
//   KHÔNG cắm dây đỏ vào Vin hay 3.3V (xem README)
//
// Hiệu ứng: cả 5 servo cùng quay 0° -> 180°, dừng, cùng quay về 0°, dừng, lặp lại.
// Góc là hàm của thời gian trong vòng (angleAt), nên 5 servo luôn nhận đúng một góc.

#include <Servo.h>

const uint8_t SERVO_PINS[] = {11, 9, 6, 5, 3};   // thứ tự từ trái qua phải
const uint8_t SERVO_COUNT  = sizeof(SERVO_PINS) / sizeof(SERVO_PINS[0]);

const int MIN_ANGLE = 0;
const int MAX_ANGLE = 180;

// ---- Thông số (đơn vị: mili giây) ----
const unsigned long DONG_BO_MOVE_MS = 2700;   // quay hết 180° (≈ 1° mỗi 15 ms như bản 04)
const unsigned long DONG_BO_HOLD_MS = 500;    // dừng ở mỗi đầu trước khi quay ngược

// Một vòng: lượt lên -> dừng ở 180° -> lượt về -> dừng ở 0°
const unsigned long DONG_BO_HALF_MS  = DONG_BO_MOVE_MS + DONG_BO_HOLD_MS;
const unsigned long DONG_BO_ROUND_MS = 2 * DONG_BO_HALF_MS;

Servo servos[SERVO_COUNT];
int lastAngle = MIN_ANGLE;                    // góc chung đã gửi, chỉ gửi lại khi thay đổi

// Góc tăng đều từ MIN_ANGLE lên MAX_ANGLE trong khoảng `duration`, tính tại thời điểm t
int rampAngle(unsigned long t, unsigned long duration) {
  if (duration == 0 || t >= duration) return MAX_ANGLE;
  return MIN_ANGLE + (long)(MAX_ANGLE - MIN_ANGLE) * t / duration;
}

// Góc chung của cả dãy tại thời điểm t trong một vòng
int angleAt(unsigned long t) {
  if (t < DONG_BO_HALF_MS) return rampAngle(t, DONG_BO_MOVE_MS);   // lượt lên rồi dừng ở 180°
  t -= DONG_BO_HALF_MS;                                            // lượt về rồi dừng ở 0°
  return MIN_ANGLE + MAX_ANGLE - rampAngle(t, DONG_BO_MOVE_MS);
}

// Gửi cùng một góc cho tất cả servo trong cùng một lượt
void writeAll(int a) {
  for (uint8_t i = 0; i < SERVO_COUNT; i++) {
    servos[i].write(a);
  }
}

void setup() {
  for (uint8_t i = 0; i < SERVO_COUNT; i++) {
    servos[i].attach(SERVO_PINS[i]);
  }
  writeAll(MIN_ANGLE);                        // đưa cả dãy về 0° khi khởi động
}

void loop() {
  int angle = angleAt(millis() % DONG_BO_ROUND_MS);
  if (angle != lastAngle) {
    writeAll(angle);
    lastAngle = angle;
  }
}
