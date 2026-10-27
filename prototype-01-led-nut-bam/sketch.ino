// Bật/tắt LED bằng nút bấm – Arduino Uno
// Mạch:
//   LED: chân dương -> điện trở 220Ω -> chân 8, chân âm -> GND
//   Nút bấm: một chân -> chân 2, chân đối diện -> GND

const uint8_t LED_PIN    = 8;
const uint8_t BUTTON_PIN = 2;
const unsigned long DEBOUNCE_MS = 50;   // thời gian lọc nhiễu nút bấm

bool ledOn = false;                     // trạng thái LED hiện tại
int  lastReading  = HIGH;               // giá trị đọc thô lần trước
int  stableState  = HIGH;               // trạng thái nút đã lọc nhiễu
unsigned long lastChangeMs = 0;         // thời điểm tín hiệu thô đổi gần nhất

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);    // điện trở kéo lên nội: nhả = HIGH, nhấn = LOW
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);

  // Tín hiệu vừa thay đổi -> bắt đầu đếm lại thời gian chờ ổn định
  if (reading != lastReading) {
    lastChangeMs = millis();
    lastReading = reading;
  }

  // Tín hiệu đã ổn định đủ lâu và khác trạng thái cũ -> chấp nhận thay đổi
  if ((millis() - lastChangeMs) > DEBOUNCE_MS && reading != stableState) {
    stableState = reading;

    // Chỉ đảo LED tại khoảnh khắc NHẤN xuống (HIGH -> LOW), bỏ qua lúc nhả
    if (stableState == LOW) {
      ledOn = !ledOn;
      digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
    }
  }
}
