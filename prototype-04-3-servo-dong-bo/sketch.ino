// 3 servo quay đồng bộ 0° <-> 180° – Arduino Uno
// Mạch (dây cam = tín hiệu):
//   Servo 1 (tay tím)   -> chân 3
//   Servo 2 (tay trắng) -> chân 5
//   Servo 3 (tay cam)   -> chân 6
//   Dây đỏ của cả 3 servo -> 5V (hoặc nguồn 5V riêng), dây nâu -> GND
//   KHÔNG cắm dây đỏ vào Vin hay 3.3V (xem README)

#include <Servo.h>

const uint8_t SERVO_PINS[]  = {3, 5, 6};
const uint8_t SERVO_COUNT   = sizeof(SERVO_PINS) / sizeof(SERVO_PINS[0]);

const int MIN_ANGLE = 0;
const int MAX_ANGLE = 180;
const unsigned long STEP_MS = 15;    // mỗi 15 ms quay 1 độ -> khoảng 2,7 giây cho 180°
const unsigned long HOLD_MS = 500;   // dừng 0,5 giây ở mỗi đầu trước khi quay ngược

Servo servos[SERVO_COUNT];
int angle = MIN_ANGLE;               // góc chung của cả 3 servo
int direction = 1;                   // +1: quay tới 180°, -1: quay về 0°
unsigned long lastStepMs = 0;
bool holding = true;                 // đang dừng ở một đầu
unsigned long holdStartMs = 0;

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
  writeAll(angle);                   // đưa cả 3 servo về 0° khi khởi động
  holdStartMs = millis();
}

void loop() {
  unsigned long now = millis();

  // Đang dừng ở đầu hành trình -> chờ đủ HOLD_MS rồi mới quay tiếp
  if (holding) {
    if (now - holdStartMs >= HOLD_MS) {
      holding = false;
      lastStepMs = now;
    }
    return;
  }

  // Mỗi STEP_MS cả 3 servo cùng quay thêm 1 độ theo hướng hiện tại
  if (now - lastStepMs >= STEP_MS) {
    lastStepMs = now;
    angle += direction;
    writeAll(angle);

    // Chạm 0° hoặc 180° -> đổi hướng và dừng một nhịp
    if (angle >= MAX_ANGLE || angle <= MIN_ANGLE) {
      direction = -direction;
      holding = true;
      holdStartMs = now;
    }
  }
}
