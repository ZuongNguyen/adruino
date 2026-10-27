// 5 servo: quay đồng bộ + lượn sóng từ trái qua phải – Arduino Uno
// Mạch (servo xếp từ trái sang phải, dây cam/vàng = tín hiệu):
//   Servo 1 -> chân 3    Servo 2 -> chân 5    Servo 3 -> chân 6
//   Servo 4 -> chân 9    Servo 5 -> chân 10
//   Dây đỏ của 5 servo -> hàng nguồn + trên breadboard (nối 5V Arduino)
//   Dây nâu/đen        -> hàng nguồn - trên breadboard (nối GND Arduino)
//   KHÔNG cắm dây đỏ vào Vin hay 3.3V (xem README)
//
// Chương trình tự chuyển qua lại giữa 2 hiệu ứng:
//   1. Đồng bộ:   cả 5 servo cùng quay 0° -> 180° -> 0°, dừng ở mỗi đầu
//   2. Lượn sóng: servo lần lượt quay lên 180° từ trái qua phải, dừng một nhịp,
//                 rồi lần lượt quay về 0° cũng từ trái qua phải
// Cả hai dùng chung một hàm angleAt(): đồng bộ chính là lượn sóng với độ trễ 0.

#include <Servo.h>

const uint8_t SERVO_PINS[] = {3, 5, 6, 9, 10};   // thứ tự từ trái qua phải
const uint8_t SERVO_COUNT  = sizeof(SERVO_PINS) / sizeof(SERVO_PINS[0]);

const int MIN_ANGLE = 0;
const int MAX_ANGLE = 180;

// ---- Số vòng chạy mỗi hiệu ứng trước khi chuyển (đặt 0 để bỏ hiệu ứng đó) ----
const uint8_t DONG_BO_ROUNDS = 2;
const uint8_t SONG_ROUNDS    = 2;

// ---- Hiệu ứng 1: đồng bộ (đơn vị: mili giây) ----
const unsigned long DONG_BO_STEP_MS = 0;      // 0 = cả 5 servo quay cùng lúc
const unsigned long DONG_BO_MOVE_MS = 2700;   // quay hết 180° (≈ 1° mỗi 15 ms như bản 04)
const unsigned long DONG_BO_HOLD_MS = 500;    // dừng ở mỗi đầu

// ---- Hiệu ứng 2: lượn sóng (đơn vị: mili giây) ----
const unsigned long SONG_STEP_MS = 300;       // servo kế bên phải bắt đầu quay trễ bao lâu
const unsigned long SONG_MOVE_MS = 1000;      // mỗi servo quay hết 180° trong bao lâu
const unsigned long SONG_HOLD_MS = 500;       // dừng khi cả dãy đã tới đầu bên kia

// Một lượt lên (hoặc về) = servo cuối bắt đầu trễ nhất + thời gian quay + thời gian dừng
const unsigned long DONG_BO_HALF_MS  = (SERVO_COUNT - 1) * DONG_BO_STEP_MS + DONG_BO_MOVE_MS + DONG_BO_HOLD_MS;
const unsigned long DONG_BO_ROUND_MS = 2 * DONG_BO_HALF_MS;
const unsigned long SONG_HALF_MS     = (SERVO_COUNT - 1) * SONG_STEP_MS + SONG_MOVE_MS + SONG_HOLD_MS;
const unsigned long SONG_ROUND_MS    = 2 * SONG_HALF_MS;

// ---- Cả chương trình: đồng bộ N vòng -> lượn sóng M vòng -> lặp lại ----
const unsigned long DONG_BO_TOTAL_MS = DONG_BO_ROUNDS * DONG_BO_ROUND_MS;
const unsigned long SHOW_MS          = DONG_BO_TOTAL_MS + SONG_ROUNDS * SONG_ROUND_MS;

Servo servos[SERVO_COUNT];
int lastAngle[SERVO_COUNT];                   // góc đã gửi, chỉ gửi lại khi thay đổi

// Góc tăng đều từ MIN_ANGLE lên MAX_ANGLE trong khoảng `duration`, tính tại thời điểm t
int rampAngle(unsigned long t, unsigned long duration) {
  if (duration == 0 || t >= duration) return MAX_ANGLE;
  return MIN_ANGLE + (long)(MAX_ANGLE - MIN_ANGLE) * t / duration;
}

// Góc của servo thứ i tại thời điểm t trong một vòng: lượt lên -> dừng -> lượt về -> dừng.
// Servo thứ i bắt đầu trễ i × stepMs so với servo đầu tiên:
//   stepMs = 0 -> cả dãy quay cùng lúc (đồng bộ)
//   stepMs > 0 -> servo quay lần lượt từ trái qua phải (lượn sóng)
int angleAt(uint8_t i, unsigned long t, unsigned long stepMs,
            unsigned long moveMs, unsigned long halfMs) {
  unsigned long start = i * stepMs;           // servo càng bên phải càng quay muộn
  if (t < halfMs) {                           // lượt lên: 0° -> 180°, rồi dừng chờ cả dãy
    return (t < start) ? MIN_ANGLE : rampAngle(t - start, moveMs);
  }
  t -= halfMs;                                // lượt về: 180° -> 0°, rồi dừng chờ cả dãy
  if (t < start) return MAX_ANGLE;
  return MIN_ANGLE + MAX_ANGLE - rampAngle(t - start, moveMs);
}

void setup() {
  for (uint8_t i = 0; i < SERVO_COUNT; i++) {
    servos[i].attach(SERVO_PINS[i]);
    servos[i].write(MIN_ANGLE);               // đưa cả dãy về 0° khi khởi động
    lastAngle[i] = MIN_ANGLE;
  }
}

void loop() {
  if (SHOW_MS == 0) return;                   // cả hai hiệu ứng đều đặt 0 vòng

  unsigned long tShow = millis() % SHOW_MS;   // vị trí hiện tại trong cả chương trình

  for (uint8_t i = 0; i < SERVO_COUNT; i++) {
    int angle;
    if (tShow < DONG_BO_TOTAL_MS) {
      angle = angleAt(i, tShow % DONG_BO_ROUND_MS,
                      DONG_BO_STEP_MS, DONG_BO_MOVE_MS, DONG_BO_HALF_MS);
    } else {
      angle = angleAt(i, (tShow - DONG_BO_TOTAL_MS) % SONG_ROUND_MS,
                      SONG_STEP_MS, SONG_MOVE_MS, SONG_HALF_MS);
    }

    if (angle != lastAngle[i]) {
      servos[i].write(angle);
      lastAngle[i] = angle;
    }
  }
}
