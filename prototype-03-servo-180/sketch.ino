// Servo quay qua lại 0° <-> 180° – Arduino Uno
// Mạch:
//   Servo dây cam (tín hiệu) -> chân 6
//   Servo dây đỏ (nguồn)     -> 5V
//   Servo dây nâu (GND)      -> GND

#include <Servo.h>

const uint8_t SERVO_PIN = 6;
const int MIN_ANGLE = 0;
const int MAX_ANGLE = 180;
const unsigned long STEP_MS = 15;    // mỗi 15 ms quay 1 độ -> khoảng 2,7 giây cho 180°
const unsigned long HOLD_MS = 500;   // dừng 0,5 giây ở mỗi đầu trước khi quay ngược

Servo servo;
int angle = MIN_ANGLE;               // góc hiện tại
int direction = 1;                   // +1: quay tới 180°, -1: quay về 0°
unsigned long lastStepMs = 0;
bool holding = true;                 // đang dừng ở một đầu
unsigned long holdStartMs = 0;

void setup() {
  servo.attach(SERVO_PIN);
  servo.write(angle);                // đưa servo về 0° khi khởi động
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

  // Mỗi STEP_MS quay thêm 1 độ theo hướng hiện tại
  if (now - lastStepMs >= STEP_MS) {
    lastStepMs = now;
    angle += direction;
    servo.write(angle);

    // Chạm 0° hoặc 180° -> đổi hướng và dừng một nhịp
    if (angle >= MAX_ANGLE || angle <= MIN_ANGLE) {
      direction = -direction;
      holding = true;
      holdStartMs = now;
    }
  }
}
