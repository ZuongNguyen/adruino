// Test servo 11/10: reset về 0° rồi quét 0° <-> 180° – Arduino Uno
// Mạch (đọc từ ảnh Tinkercad, dây xanh lá = tín hiệu):
//   Servo dây cam (tín hiệu) -> chân 3
//   Servo dây đỏ (nguồn)     -> hàng nguồn + dưới breadboard -> 5V Arduino
//   Servo dây nâu (GND)      -> hàng nguồn - dưới breadboard -> GND Arduino
//   KHÔNG cắm dây đỏ vào Vin hay 3.3V (xem README)
//
// Hiệu ứng: mỗi lần cấp điện hoặc nhấn nút RESET đỏ trên Arduino, servo về 0° và đứng yên RESET_MS
// cho kịp quay về từ bất kỳ góc nào đang đứng, rồi quét 0° -> 180° -> 0° mãi, dừng ở mỗi đầu.
// Góc là hàm của thời gian trong vòng quét (angleAt), loop() không bao giờ chặn. Mốc vòng dời bằng phép trừ
// unsigned nên khi millis() tràn về 0 (sau ≈ 49,7 ngày) servo vẫn quét tiếp, không quay lại giai đoạn reset.

#include <Servo.h>

const uint8_t SERVO_PIN = 3;

const int MIN_ANGLE = 0;
const int MAX_ANGLE = 180;

// ---- Thông số (đơn vị: mili giây) ----
const unsigned long RESET_MS     = 1000;   // đứng ở 0° sau khi khởi động (SG90 quay 180° chỉ mất ≈ 0,3–0,5 s)
const unsigned long QUET_MOVE_MS = 2700;   // quay hết 180° (≈ 1° mỗi 15 ms như bản 03)
const unsigned long QUET_HOLD_MS = 500;    // dừng ở mỗi đầu trước khi quay ngược

// Một vòng quét: lượt lên -> dừng ở 180° -> lượt về -> dừng ở 0°
const unsigned long QUET_HALF_MS  = QUET_MOVE_MS + QUET_HOLD_MS;
const unsigned long QUET_ROUND_MS = 2 * QUET_HALF_MS;

Servo servo;
int lastAngle = MIN_ANGLE;                 // góc đã gửi, chỉ gửi lại khi thay đổi
unsigned long mocKhoiDong = 0;             // millis() lúc setup(), mốc của giai đoạn reset
bool dangQuet = false;                     // đã qua giai đoạn reset (không quay lại khi millis() tràn)
unsigned long mocVong = 0;                 // millis() lúc bắt đầu vòng quét hiện tại

// Góc tăng đều từ MIN_ANGLE lên MAX_ANGLE trong khoảng `duration`, tính tại thời điểm t
int rampAngle(unsigned long t, unsigned long duration) {
  if (duration == 0 || t >= duration) return MAX_ANGLE;
  return MIN_ANGLE + (long)(MAX_ANGLE - MIN_ANGLE) * t / duration;
}

// Góc servo tại thời điểm t (ms) tính từ đầu một vòng quét, 0 <= t < QUET_ROUND_MS
int angleAt(unsigned long t) {
  if (t < QUET_HALF_MS) return rampAngle(t, QUET_MOVE_MS);         // lượt lên rồi dừng ở 180°
  t -= QUET_HALF_MS;                                               // lượt về rồi dừng ở 0°
  return MIN_ANGLE + MAX_ANGLE - rampAngle(t, QUET_MOVE_MS);
}

void setup() {
  servo.attach(SERVO_PIN);
  servo.write(MIN_ANGLE);                  // reset: đưa servo về 0° ngay khi khởi động
  mocKhoiDong = millis();
}

void loop() {
  unsigned long now = millis();
  // Hết RESET_MS thì bắt đầu quét; vòng đầu tính từ đúng lúc hết reset
  if (!dangQuet && QUET_ROUND_MS > 0 && now - mocKhoiDong >= RESET_MS) {
    dangQuet = true;
    mocVong = mocKhoiDong + RESET_MS;
  }

  int angle = MIN_ANGLE;                   // giai đoạn reset: đứng ở 0°
  if (dangQuet) {
    unsigned long t = (now - mocVong) % QUET_ROUND_MS;   // phép trừ unsigned: vẫn đúng khi millis() tràn
    mocVong = now - t;                                   // dời mốc về đầu vòng hiện tại
    angle = angleAt(t);
  }

  if (angle != lastAngle) {
    servo.write(angle);
    lastAngle = angle;
  }
}
