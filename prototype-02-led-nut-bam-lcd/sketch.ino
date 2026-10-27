// Bật/tắt LED bằng nút bấm + hiển thị chữ trên LCD 16x2 I2C – Arduino Uno
// Mạch:
//   LED: chân dương -> điện trở 220Ω -> chân 8, chân âm -> GND
//   Nút bấm: một chân -> chân 2, chân đối diện -> GND
//   LCD I2C: GND -> GND, VCC -> 5V, SDA -> A4, SCL -> A5

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const uint8_t LED_PIN    = 8;
const uint8_t BUTTON_PIN = 2;
const unsigned long DEBOUNCE_MS = 50;   // thời gian lọc nhiễu nút bấm

const uint8_t LCD_COLS = 16;
const uint8_t LCD_ROWS = 2;
LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS);   // 0x27 là địa chỉ I2C mặc định

const char TITLE[] = " Dai Duong ";

bool ledOn = false;                     // trạng thái LED hiện tại
int  lastReading  = HIGH;               // giá trị đọc thô lần trước
int  stableState  = HIGH;               // trạng thái nút đã lọc nhiễu
unsigned long lastChangeMs = 0;         // thời điểm tín hiệu thô đổi gần nhất

// In chữ căn giữa trên một dòng
void printCentered(uint8_t row, const char* text) {
  uint8_t len = strlen(text);
  uint8_t col = (len < LCD_COLS) ? (LCD_COLS - len) / 2 : 0;
  lcd.setCursor(col, row);
  lcd.print(text);
}

// Ghi đè cả dòng 2 (đủ 16 ký tự) để không còn sót chữ cũ
void showLedStatus() {
  lcd.setCursor(0, 1);
  lcd.print(ledOn ? "    LED: BAT    " : "    LED: TAT    ");
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);    // điện trở kéo lên nội: nhả = HIGH, nhấn = LOW
  digitalWrite(LED_PIN, LOW);

  lcd.init();
  lcd.backlight();
  printCentered(0, TITLE);              // dòng 1: " Dai Duong "
  showLedStatus();                      // dòng 2: trạng thái LED
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
      showLedStatus();                  // chỉ cập nhật LCD khi có thay đổi
    }
  }
}
