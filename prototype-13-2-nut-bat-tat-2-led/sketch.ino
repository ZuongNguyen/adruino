// 2 nút bấm bật/tắt 2 LED – Arduino Uno
// Mạch (đọc từ ảnh Tinkercad, linh kiện đếm từ trái sang phải trên breadboard):
//   Nút 1 -> chân 2: một chân -> chân 2 và qua điện trở kéo xuống -> GND, chân kia -> 5V
//                    => nhả = LOW, nhấn = HIGH
//   Nút 2 -> chân 3: một chân -> chân 3, chân kia -> GND, không điện trở
//                    => dùng INPUT_PULLUP: nhả = HIGH, nhấn = LOW
//   LED đỏ  : chân dương -> chân 4, chân âm -> điện trở -> GND
//   LED xanh: chân dương -> chân 5, chân âm -> điện trở -> GND
//   5V và GND của Arduino -> hàng + và hàng - dưới cùng của breadboard
//
// Hoạt động:
//   Nhấn nút 1: LED đỏ đổi trạng thái (đang tắt thì bật, đang bật thì tắt)
//   Nhấn nút 2: LED xanh đổi trạng thái
//   Chỉ tính lúc nhấn xuống, giữ nút không làm đèn đổi nữa. Mới bật nguồn hai LED đều tắt.

// ===== Thông số chỉnh được =====
const uint8_t BUTTON_PINS[] = {2, 3};        // nút 1, nút 2
const uint8_t LED_PINS[]    = {4, 5};        // LED đỏ (nút 1 bật/tắt), LED xanh (nút 2 bật/tắt)
const int PRESSED[]         = {HIGH, LOW};   // mức đọc được khi nhấn từng nút:
                                             //   HIGH: nút có điện trở kéo xuống GND, chân kia lên 5V
                                             //   LOW : nút nối thẳng chân <-> GND, code tự dùng INPUT_PULLUP
const unsigned long DEBOUNCE_MS = 50;        // thời gian lọc nhiễu nút bấm

const uint8_t BUTTON_COUNT = sizeof(BUTTON_PINS) / sizeof(BUTTON_PINS[0]);   // mỗi nút một LED

bool ledOn[BUTTON_COUNT];                    // LED của nút b đang sáng hay tắt
int  lastReading[BUTTON_COUNT];              // giá trị đọc thô lần trước
int  stableState[BUTTON_COUNT];              // trạng thái nút đã lọc nhiễu
unsigned long lastChangeMs[BUTTON_COUNT];    // thời điểm tín hiệu thô đổi gần nhất

void setup() {
  for (uint8_t b = 0; b < BUTTON_COUNT; b++) {
    pinMode(LED_PINS[b], OUTPUT);
    ledOn[b] = false;
    digitalWrite(LED_PINS[b], LOW);
    pinMode(BUTTON_PINS[b], PRESSED[b] == HIGH ? INPUT : INPUT_PULLUP);
    lastReading[b] = stableState[b] = (PRESSED[b] == HIGH) ? LOW : HIGH;   // coi như đang nhả
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
      if (stableState[b] == PRESSED[b]) {          // chỉ tính lúc nhấn, bỏ qua lúc nhả
        ledOn[b] = !ledOn[b];
        digitalWrite(LED_PINS[b], ledOn[b] ? HIGH : LOW);
      }
    }
  }
}
