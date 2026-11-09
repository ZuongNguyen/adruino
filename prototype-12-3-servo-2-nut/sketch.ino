// Bài 1.4: 3 servo điều khiển bằng 2 nút bấm – Arduino Uno
// Mạch (đọc từ ảnh Tinkercad, servo đếm từ trái sang phải, dây xanh lá = tín hiệu):
//   Servo 1 -> chân 9    Servo 2 -> chân 10    Servo 3 -> chân 11
//   Dây đỏ của 3 servo -> hàng + trên breadboard (nối 5V Arduino)
//   Dây đen của 3 servo -> hàng - trên breadboard (nối GND Arduino)
//   Nút 1 (nút dưới) -> chân 3, Nút 2 (nút trên) -> chân 2
//   Mỗi nút: một chân -> chân Arduino, chân chéo đối diện -> hàng - (GND), không điện trở
//   => dùng INPUT_PULLUP: nhả = HIGH, nhấn = LOW
//
// Hoạt động (nhấn một lần là chạy liên tục, không cần giữ nút):
//   Nút 1: servo 1, 2 quay 0° <-> 90°, servo 3 quay 0° <-> 180°, đổi chiều mỗi 0,5 giây
//   Nút 2: servo 1, 2 quay 0° <-> 180°, servo 3 quay 0° <-> 90°, đổi chiều mỗi 0,5 giây
//   Lúc mới bật nguồn cả 3 servo đứng ở 0°. Nhấn lại nút đang chạy thì giữ nguyên nhịp.
//   Đổi nút: servo luôn về 0° và đứng đủ 0,5 giây rồi mới quay tới góc mới
//     - đang ở góc quay tới -> về 0° ngay
//     - đang ở 0°           -> đứng nốt cho đủ 0,5 giây như nhịp cũ

#include <Servo.h>

// ===== Thông số chỉnh được =====
const uint8_t SERVO_PINS[]  = {9, 10, 11};    // thứ tự từ trái qua phải
const uint8_t BUTTON_PINS[] = {3, 2};         // nút 1, nút 2
const uint8_t SERVO_COUNT   = sizeof(SERVO_PINS) / sizeof(SERVO_PINS[0]);
const uint8_t BUTTON_COUNT  = sizeof(BUTTON_PINS) / sizeof(BUTTON_PINS[0]);

const int REST_ANGLE = 0;                     // góc đứng chờ và góc quay về

// Góc quay tới của từng servo (trái -> phải) khi chạy theo từng nút
const int OUT_ANGLES[BUTTON_COUNT][SERVO_COUNT] = {
  {90,  90,  180},                            // nút 1: 2 servo quay 90°, 1 servo quay 180°
  {180, 180, 90},                             // nút 2: 2 servo quay 180°, 1 servo quay 90°
};

const unsigned long STEP_MS     = 500;        // giữ mỗi góc bao lâu rồi đổi chiều
const unsigned long DEBOUNCE_MS = 50;         // thời gian lọc nhiễu nút bấm
const int PRESSED = LOW;    // nút nối chân <-> GND, không điện trở (như ảnh). Nếu nút có
                            // điện trở kéo xuống GND và chân kia lên 5V thì đổi thành HIGH

// Một vòng = quay tới, giữ STEP_MS, quay về, giữ STEP_MS
const unsigned long ROUND_MS = 2 * STEP_MS;

const int8_t DUNG_YEN = -1;                   // chưa nhấn nút nào

Servo servos[SERVO_COUNT];
int lastAngle[SERVO_COUNT];                   // góc đã gửi, chỉ gửi lại khi thay đổi
int8_t mode = DUNG_YEN;                       // DUNG_YEN, hoặc số thứ tự nút đang chạy (0 = nút 1)
unsigned long roundStartMs = 0;               // thời điểm bắt đầu vòng đầu tiên của chế độ hiện tại

int  lastReading[BUTTON_COUNT];               // giá trị đọc thô lần trước
int  stableState[BUTTON_COUNT];               // trạng thái nút đã lọc nhiễu
unsigned long lastChangeMs[BUTTON_COUNT];     // thời điểm tín hiệu thô đổi gần nhất

// Nửa đầu vòng servo ở góc quay tới, nửa sau ở góc quay về
bool isOut(unsigned long t) {
  if (ROUND_MS == 0) return false;            // STEP_MS = 0: servo đứng yên ở 0°
  return (t % ROUND_MS) < STEP_MS;
}

// Góc của servo thứ i khi chạy theo chế độ m, t = thời gian từ đầu vòng đầu tiên
int angleAt(int8_t m, uint8_t i, unsigned long t) {
  if (m == DUNG_YEN) return REST_ANGLE;
  return isOut(t) ? OUT_ANGLES[m][i] : REST_ANGLE;
}

// Xử lý khoảnh khắc nhấn xuống của nút thứ b (0 = nút 1)
void onPress(uint8_t b, unsigned long now) {
  if (b == mode) return;                      // nhấn lại nút đang chạy: giữ nguyên nhịp
  if (mode == DUNG_YEN) {
    roundStartMs = now;                       // đang đứng yên: quay tới góc mới ngay
  } else if (isOut(now - roundStartMs)) {
    roundStartMs = now - STEP_MS;             // đang ở góc quay tới: vào nửa quay về từ bây giờ
  }                                           // đang ở 0°: giữ nhịp, hết nửa quay về mới quay tới
  mode = b;
}

void setup() {
  for (uint8_t i = 0; i < SERVO_COUNT; i++) {
    servos[i].attach(SERVO_PINS[i]);
    servos[i].write(REST_ANGLE);              // đưa cả dãy về 0° khi khởi động
    lastAngle[i] = REST_ANGLE;
  }
  for (uint8_t b = 0; b < BUTTON_COUNT; b++) {
    pinMode(BUTTON_PINS[b], PRESSED == HIGH ? INPUT : INPUT_PULLUP);
    lastReading[b] = stableState[b] = (PRESSED == HIGH) ? LOW : HIGH;   // coi như đang nhả
    lastChangeMs[b] = 0;
  }
}

void loop() {
  unsigned long now = millis();
  for (uint8_t b = 0; b < BUTTON_COUNT; b++) {
    int reading = digitalRead(BUTTON_PINS[b]);

    // Tín hiệu vừa thay đổi -> bắt đầu đếm lại thời gian chờ ổn định
    if (reading != lastReading[b]) {
      lastChangeMs[b] = now;
      lastReading[b] = reading;
    }

    // Tín hiệu đã ổn định đủ lâu và khác trạng thái cũ -> chấp nhận thay đổi
    if ((now - lastChangeMs[b]) > DEBOUNCE_MS && reading != stableState[b]) {
      stableState[b] = reading;
      if (stableState[b] == PRESSED) onPress(b, now);   // chỉ tính lúc nhấn, bỏ qua lúc nhả
    }
  }

  for (uint8_t i = 0; i < SERVO_COUNT; i++) {
    int angle = angleAt(mode, i, now - roundStartMs);
    if (angle != lastAngle[i]) {
      servos[i].write(angle);
      lastAngle[i] = angle;
    }
  }
}
