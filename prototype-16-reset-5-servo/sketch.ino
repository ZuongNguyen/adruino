// Reset 5 servo về 0° lần lượt từ servo 1 đến servo 5 – Arduino Uno
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
// Hiệu ứng: mỗi lần cấp điện hoặc nhấn nút RESET đỏ trên Arduino, servo 1 về 0° trước, cứ sau RESET_GAP_MS
// thì tới servo kế tiếp, đến servo 5. Sau đó cả 5 đứng yên ở 0°.
// Servo chưa tới lượt thì chưa attach(): chân chưa có xung nên servo đứng yên chỗ cũ, chưa kéo dòng để quay.
// Lần lượt như vậy để 5 servo không cùng kéo dòng một lúc (dễ sụt nguồn, Arduino tự reset).

#include <Servo.h>

const uint8_t SERVO_PINS[] = {5, 6, 9, 10, 11};   // servo 1 -> 5 theo quy ước của người dùng
const uint8_t SERVO_COUNT  = sizeof(SERVO_PINS) / sizeof(SERVO_PINS[0]);

const int RESET_ANGLE = 0;

// ---- Thông số (đơn vị: mili giây) ----
const unsigned long RESET_GAP_MS = 500;   // servo sau bắt đầu về 0° muộn hơn servo trước bấy nhiêu
                                          // (SG90 quay 180° mất ≈ 0,3–0,5 s; đặt 0 để cả 5 về cùng lúc)

Servo servos[SERVO_COUNT];
bool daReset[SERVO_COUNT];                // servo đã nhận tín hiệu 0° chưa
unsigned long mocKhoiDong = 0;            // millis() lúc setup()

void setup() {
  for (uint8_t i = 0; i < SERVO_COUNT; i++) {
    // Ghi góc TRƯỚC khi attach(): xung đầu tiên đã là 0°, servo không bị kéo qua 90° mặc định của thư viện
    servos[i].write(RESET_ANGLE);
    daReset[i] = false;
  }
  mocKhoiDong = millis();
}

void loop() {
  unsigned long daQua = millis() - mocKhoiDong;
  for (uint8_t i = 0; i < SERVO_COUNT; i++) {
    // Tới lượt servo thứ i (tính từ 0) thì attach(): servo nhận xung 0° và quay về, rồi giữ ở đó
    if (!daReset[i] && daQua >= (unsigned long)i * RESET_GAP_MS) {
      servos[i].attach(SERVO_PINS[i]);
      daReset[i] = true;
    }
  }
}
