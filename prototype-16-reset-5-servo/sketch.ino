// Reset 5 servo về 0° lần lượt, rồi servo 2 quay chậm lên 45°, servo 3 lên 60° – Arduino Uno
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
//   3. Quay chậm lần lượt theo bảng MOVES[]: servo 2 lên 45°, nghỉ MOVE_PAUSE_MS, rồi servo 3 lên 60°.
//      Mọi servo quay cùng tốc độ MOVE_SPEED_DEG_S (15°/giây: 45° mất 3 s, 60° mất 4 s).
//      Servo 1, 4, 5 giữ 0°. Quay xong thì cả 5 đứng yên.
// Với thông số mặc định: reset xong lúc 2 s, servo 2 quay từ 2,5 s đến 5,5 s, servo 3 quay từ 6 s đến 10 s.

#include <Servo.h>

const uint8_t SERVO_PINS[] = {5, 6, 9, 10, 11};   // servo 1 -> 5 theo quy ước của người dùng
const uint8_t SERVO_COUNT  = sizeof(SERVO_PINS) / sizeof(SERVO_PINS[0]);

const int RESET_ANGLE = 0;

// Bảng quay chậm, chạy lần lượt từ trên xuống: { số servo của người dùng (1 -> 5), góc quay tới }
struct BuocQuay {
  uint8_t servo;
  int goc;
};
const BuocQuay MOVES[] = {
  {2, 45},   // servo 2 lên 45°
  {3, 60},   // rồi servo 3 lên 60°
};
const uint8_t MOVE_COUNT = sizeof(MOVES) / sizeof(MOVES[0]);

// ---- Thông số ----
const unsigned long RESET_GAP_MS  = 500;   // ms: servo sau bắt đầu về 0° muộn hơn servo trước bấy nhiêu
                                           // (SG90 quay 180° mất ≈ 0,3–0,5 s; đặt 0 để cả 5 về cùng lúc)
const unsigned long RESET_HOLD_MS = 500;   // ms: chờ thêm sau khi servo cuối nhận lệnh 0°, cho nó kịp về tới nơi
const unsigned long MOVE_SPEED_DEG_S = 15; // tốc độ quay chậm, độ mỗi giây: càng nhỏ càng chậm (0 = hết tốc độ)
const unsigned long MOVE_PAUSE_MS = 500;   // ms: nghỉ giữa lúc servo trước quay xong và servo sau bắt đầu quay

Servo servos[SERVO_COUNT];
bool daReset[SERVO_COUNT];                 // servo đã nhận tín hiệu 0° chưa
int lastAngle[SERVO_COUNT];                // góc đã ghi gần nhất, chỉ ghi lại khi góc đổi
unsigned long moveBatDau[MOVE_COUNT];      // lúc bắt đầu quay của từng bước, tính từ đầu bước quay chậm
unsigned long moveThoiGian[MOVE_COUNT];    // thời gian quay của từng bước (ms), tính từ góc và tốc độ
unsigned long moveKetThuc = 0;             // lúc bước cuối quay xong
bool xong = false;                         // đã quay xong hết, loop() không còn việc gì
unsigned long mocKhoiDong = 0;             // millis() lúc setup()

// Góc của bước thứ k trong MOVES[], t = thời gian tính từ lúc bắt đầu bước quay chậm
int moveAngleAt(uint8_t k, unsigned long t) {
  if (t < moveBatDau[k]) return RESET_ANGLE;                // chưa tới lượt
  unsigned long daQuay = t - moveBatDau[k];
  if (daQuay >= moveThoiGian[k]) return MOVES[k].goc;      // đã quay xong (thời gian 0 thì tới luôn)
  return RESET_ANGLE + (long)(MOVES[k].goc - RESET_ANGLE) * (long)daQuay / (long)moveThoiGian[k];
}

void setup() {
  for (uint8_t i = 0; i < SERVO_COUNT; i++) {
    // Ghi góc TRƯỚC khi attach(): xung đầu tiên đã là 0°, servo không bị kéo qua 90° mặc định của thư viện
    servos[i].write(RESET_ANGLE);
    lastAngle[i] = RESET_ANGLE;
    daReset[i] = false;
  }

  // Lập lịch quay chậm: bước sau bắt đầu khi bước trước quay xong được MOVE_PAUSE_MS
  unsigned long moc = 0;
  for (uint8_t k = 0; k < MOVE_COUNT; k++) {
    int doQuay = MOVES[k].goc - RESET_ANGLE;
    if (doQuay < 0) doQuay = -doQuay;
    moveBatDau[k]   = moc;
    moveThoiGian[k] = MOVE_SPEED_DEG_S > 0 ? (unsigned long)doQuay * 1000UL / MOVE_SPEED_DEG_S : 0;
    moveKetThuc     = moc + moveThoiGian[k];
    moc = moveKetThuc + MOVE_PAUSE_MS;
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
    uint8_t i = MOVES[k].servo - 1;        // số servo 1 -> 5 đổi thành chỉ số mảng 0 -> 4
    if (i >= SERVO_COUNT) continue;        // ghi nhầm số servo (0 hoặc lớn hơn 5) thì bỏ qua
    int angle = moveAngleAt(k, t);
    if (angle != lastAngle[i]) {
      servos[i].write(angle);
      lastAngle[i] = angle;
    }
  }

  // Bước cuối đã quay xong (góc cuối đã ghi ở vòng for trên) thì dừng hẳn; không tính khoảng nghỉ sau nó
  if (t >= moveKetThuc) xong = true;
}
