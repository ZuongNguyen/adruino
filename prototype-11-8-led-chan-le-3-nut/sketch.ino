// Bài 1.3: 8 LED chẵn/lẻ điều khiển bằng 3 nút bấm – Arduino Uno
// Mạch (đọc từ ảnh Tinkercad, LED và nút đếm từ trái sang phải):
//   LED 1 vàng      : chân dương -> chân 13, chân âm -> GND
//   LED 2 xanh dương: chân dương -> chân 12, chân âm -> GND
//   LED 3 xanh lá   : chân dương -> chân 11, chân âm -> GND
//   LED 4 vàng      : chân dương -> chân 10, chân âm -> GND
//   LED 5 cam       : chân dương -> chân 9,  chân âm -> GND
//   LED 6 trắng     : chân dương -> chân 8,  chân âm -> GND
//   LED 7 xanh lá   : chân dương -> chân 7,  chân âm -> GND
//   LED 8 xanh dương: chân dương -> chân 6,  chân âm -> GND
//   (mạch thật: mỗi LED thêm điện trở 220Ω nối tiếp)
//   Nút 1 -> chân 4, Nút 2 -> chân 3, Nút 3 -> chân 2
//   Mỗi nút: một chân -> 5V, chân kia -> chân Arduino và qua điện trở kéo xuống -> GND
//   => nhả = LOW, nhấn = HIGH
//
// Hoạt động (LED lẻ = LED 1, 3, 5, 7; LED chẵn = LED 2, 4, 6, 8 – trùng với số chân 13, 11, 9, 7 lẻ
// và 12, 10, 8, 6 chẵn):
//   Nút 1: LED chẵn sáng, LED lẻ tắt
//   Nút 2: LED lẻ sáng, LED chẵn tắt
//   Nút 3: đảo – chẵn đang sáng thì sang lẻ, lẻ đang sáng thì sang chẵn
//          (lúc mới bật nguồn mọi LED tắt, nhấn nút 3 thì chẵn sáng trước)

// ===== Thông số chỉnh được =====
const uint8_t LED_PINS[]    = {13, 12, 11, 10, 9, 8, 7, 6};  // trái -> phải
const uint8_t BUTTON_PINS[] = {4, 3, 2};                     // nút 1, 2, 3
const unsigned long DEBOUNCE_MS = 50;                        // thời gian lọc nhiễu nút bấm
const int PRESSED = HIGH;   // nút có điện trở kéo xuống GND (như ảnh). Nếu nút nối chân <-> GND
                            // và không có điện trở thì đổi thành LOW (tự dùng INPUT_PULLUP)

const uint8_t LED_COUNT    = sizeof(LED_PINS) / sizeof(LED_PINS[0]);
const uint8_t BUTTON_COUNT = sizeof(BUTTON_PINS) / sizeof(BUTTON_PINS[0]);

// Chế độ đèn
const uint8_t TAT_HET = 0;
const uint8_t CHAN    = 1;   // LED chẵn sáng
const uint8_t LE      = 2;   // LED lẻ sáng

uint8_t mode = TAT_HET;
int  lastReading[BUTTON_COUNT];              // giá trị đọc thô lần trước
int  stableState[BUTTON_COUNT];              // trạng thái nút đã lọc nhiễu
unsigned long lastChangeMs[BUTTON_COUNT];    // thời điểm tín hiệu thô đổi gần nhất

// LED thứ i (đếm từ 0) là LED số i + 1: i lẻ -> LED chẵn
bool ledOnIn(uint8_t m, uint8_t i) {
  bool chan = (i % 2 == 1);
  if (m == CHAN) return chan;
  if (m == LE)   return !chan;
  return false;
}

void showMode() {
  for (uint8_t i = 0; i < LED_COUNT; i++)
    digitalWrite(LED_PINS[i], ledOnIn(mode, i) ? HIGH : LOW);
}

// Xử lý khoảnh khắc nhấn xuống của nút thứ b (0 = nút 1)
void onPress(uint8_t b) {
  if (b == 0)      mode = CHAN;
  else if (b == 1) mode = LE;
  else             mode = (mode == CHAN) ? LE : CHAN;
  showMode();
}

void setup() {
  for (uint8_t i = 0; i < LED_COUNT; i++) pinMode(LED_PINS[i], OUTPUT);
  for (uint8_t b = 0; b < BUTTON_COUNT; b++) {
    pinMode(BUTTON_PINS[b], PRESSED == HIGH ? INPUT : INPUT_PULLUP);
    lastReading[b] = stableState[b] = (PRESSED == HIGH) ? LOW : HIGH;   // coi như đang nhả
    lastChangeMs[b] = 0;
  }
  showMode();
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
      if (stableState[b] == PRESSED) onPress(b);   // chỉ tính lúc nhấn, bỏ qua lúc nhả
    }
  }
}
