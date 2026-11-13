// Reset 5 servo về 0° lần lượt, rồi servo 2, 3 quay chậm lên 45° lần lượt – Arduino Uno
// Mạch (đọc từ ảnh Tinkercad, servo xếp dọc bên phải Arduino). Quy ước của người dùng: servo 1 → 5
// theo chân tín hiệu 5, 6, 9, 10, 11, nên servo 1 là servo DƯỚI CÙNG trong ảnh, servo 5 trên cùng:
//   Servo 1 (dưới cùng) dây tín hiệu -> chân 5   (dây cam)
//   Servo 2                          -> chân 6   (dây xanh lá)
//   Servo 3 (giữa)                   -> chân 9   (dây xanh nhạt)
//   Servo 4                          -> chân 10  (dây tím)
//   Servo 5 (trên cùng)              -> chân 11  (dây hồng)
//   Dây đỏ của 5 servo  -> hàng nguồn + dưới breadboard (nối 5V Arduino)
//   Dây nâu của 5 servo -> hàng nguồn - dưới breadboard (nối GND Arduino)
//   KHÔNG cắm dây đỏ vào Vin hay 3.3V. Mạch thật 5 servo cần nguồn 5V riêng, nối chung GND (xem README)
//
// Hiệu ứng, mỗi lần cấp điện hoặc nhấn nút RESET đỏ trên Arduino:
//   1. Reset: servo 1 về 0° trước, cứ sau RESET_GAP_MS thì tới servo kế tiếp, đến servo 5.
//      Servo chưa tới lượt thì chưa attach(): chân chưa có xung nên servo đứng yên chỗ cũ, chưa kéo dòng để quay.
//      Lần lượt như vậy để 5 servo không cùng kéo dòng một lúc (dễ sụt nguồn, Arduino tự reset).
//      Bước này servo quay hết tốc độ, vì chưa biết lúc đầu servo đang đứng ở góc nào để quay chậm từ đó.
//   2. Chờ RESET_HOLD_MS cho servo 5 kịp về tới 0°.
//   3. Quay chậm: servo 2 quay từ 0° lên 45° trong MOVE_MS (3 giây), nghỉ MOVE_PAUSE_MS, rồi tới servo 3.
//      Servo 1, 4, 5 giữ 0°. Quay xong thì cả 5 đứng yên.
// Với thông số mặc định: reset xong lúc 2 s, servo 2 quay từ 2,5 s đến 5,5 s, servo 3 quay từ 6 s đến 9 s.

#include <Servo.h>

const uint8_t SERVO_PINS[] = {5, 6, 9, 10, 11};   // servo 1 -> 5 theo quy ước của người dùng
const uint8_t SERVO_COUNT  = sizeof(SERVO_PINS) / sizeof(SERVO_PINS[0]);

const int RESET_ANGLE = 0;

// Servo quay chậm, ghi theo SỐ servo của người dùng (1 -> 5); quay lần lượt theo thứ tự trong mảng
const uint8_t MOVE_SERVOS[] = {2, 3};
const uint8_t MOVE_COUNT    = sizeof(MOVE_SERVOS) / sizeof(MOVE_SERVOS[0]);
const int MOVE_ANGLE = 45;                 // góc quay tới

// ---- Thông số (đơn vị: mili giây) ----
const unsigned long RESET_GAP_MS  = 500;   // servo sau bắt đầu về 0° muộn hơn servo trước bấy nhiêu
                                           // (SG90 quay 180° mất ≈ 0,3–0,5 s; đặt 0 để cả 5 về cùng lúc)
const unsigned long RESET_HOLD_MS = 500;   // chờ thêm sau khi servo cuối nhận lệnh 0°, cho nó kịp về tới nơi
const unsigned long MOVE_MS       = 3000;  // thời gian quay hết 0° -> 45°: càng lớn càng chậm (0 = hết tốc độ)
const unsigned long MOVE_PAUSE_MS = 500;   // nghỉ giữa lúc servo trước quay xong và servo sau bắt đầu quay

Servo servos[SERVO_COUNT];
bool daReset[SERVO_COUNT];                 // servo đã nhận tín hiệu 0° chưa
int lastAngle[SERVO_COUNT];                // góc đã ghi gần nhất, chỉ ghi lại khi góc đổi
bool xong = false;                         // đã quay xong hết, loop() không còn việc gì
unsigned long mocKhoiDong = 0;             // millis() lúc setup()

// Góc của servo thứ k trong MOVE_SERVOS, t = thời gian tính từ lúc bắt đầu bước quay chậm
int moveAngleAt(uint8_t k, unsigned long t) {
  unsigned long batDau = (unsigned long)k * (MOVE_MS + MOVE_PAUSE_MS);
  if (t < batDau) return RESET_ANGLE;               // chưa tới lượt
  if (t - batDau >= MOVE_MS) return MOVE_ANGLE;     // đã quay xong (MOVE_MS = 0 thì tới luôn)
  long chia = MOVE_MS > 0 ? (long)MOVE_MS : 1;      // tới đây thì MOVE_MS > 0; viết vậy để trình biên dịch không báo chia cho 0
  return RESET_ANGLE + (long)(MOVE_ANGLE - RESET_ANGLE) * (long)(t - batDau) / chia;
}

void setup() {
  for (uint8_t i = 0; i < SERVO_COUNT; i++) {
    // Ghi góc TRƯỚC khi attach(): xung đầu tiên đã là 0°, servo không bị kéo qua 90° mặc định của thư viện
    servos[i].write(RESET_ANGLE);
    lastAngle[i] = RESET_ANGLE;
    daReset[i] = false;
  }
  mocKhoiDong = millis();
}

void loop() {
  if (xong) return;
  unsigned long daQua = millis() - mocKhoiDong;

  // 1. Reset: tới lượt servo thứ i (tính từ 0) thì attach(), servo nhận xung 0° và quay về
  for (uint8_t i = 0; i < SERVO_COUNT; i++) {
    if (!daReset[i] && daQua >= (unsigned long)i * RESET_GAP_MS) {
      servos[i].attach(SERVO_PINS[i]);
      daReset[i] = true;
    }
  }

  // 2. Quay chậm, bắt đầu sau khi servo cuối nhận lệnh 0° được RESET_HOLD_MS
  unsigned long moveStart = (unsigned long)(SERVO_COUNT - 1) * RESET_GAP_MS + RESET_HOLD_MS;
  if (daQua < moveStart) return;
  unsigned long t = daQua - moveStart;

  for (uint8_t k = 0; k < MOVE_COUNT; k++) {
    uint8_t i = MOVE_SERVOS[k] - 1;        // số servo 1 -> 5 đổi thành chỉ số mảng 0 -> 4
    if (i >= SERVO_COUNT) continue;        // ghi nhầm số servo (0 hoặc lớn hơn 5) thì bỏ qua
    int angle = moveAngleAt(k, t);
    if (angle != lastAngle[i]) {
      servos[i].write(angle);
      lastAngle[i] = angle;
    }
  }

  // Servo cuối đã tới góc đích (góc cuối đã ghi ở vòng for trên) thì dừng hẳn.
  // Servo cuối bắt đầu lúc (MOVE_COUNT - 1) × (MOVE_MS + MOVE_PAUSE_MS), quay thêm MOVE_MS; không tính khoảng nghỉ sau nó.
  if (MOVE_COUNT == 0 ||
      t >= (unsigned long)(MOVE_COUNT - 1) * (MOVE_MS + MOVE_PAUSE_MS) + MOVE_MS) {
    xong = true;
  }
}
