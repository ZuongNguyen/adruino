// 6 LED chớp sáng rồi tắt dần, sáng tắt so le nhau – Arduino Uno
// Mạch (LED xếp từ trái sang phải trên breadboard):
//   LED 1 -> ~11   LED 2 -> ~10   LED 3 -> ~9
//   LED 4 -> ~6    LED 5 -> ~5    LED 6 -> ~3
//   Chân dương (chân cong) nối chân PWM, chân âm nối hàng GND trên breadboard
//   Mạch thật: thêm điện trở 220Ω nối tiếp mỗi LED
//
// Hiệu ứng: nhóm LED lẻ (1, 3, 5) chớp sáng rồi tắt dần,
// sau đó tới nhóm LED chẵn (2, 4, 6), cứ thế luân phiên.

const uint8_t LED_PINS[] = {11, 10, 9, 6, 5, 3};
const uint8_t LED_COUNT  = sizeof(LED_PINS) / sizeof(LED_PINS[0]);

// Chỉnh hiệu ứng ở đây (đơn vị: mili giây)
const unsigned long ATTACK_MS = 0;     // thời gian sáng lên: 0 = chớp sáng ngay, >0 = sáng dần
const unsigned long DECAY_MS  = 700;   // thời gian tắt dần
const unsigned long GAP_MS    = 100;   // tắt hẳn một chút trước khi nhóm kia chớp

const unsigned long HALF_MS   = ATTACK_MS + DECAY_MS + GAP_MS;   // lượt của một nhóm
const unsigned long PERIOD_MS = 2 * HALF_MS;                     // một vòng lẻ + chẵn

int lastPwm[LED_COUNT];                // giá trị đã ghi, chỉ ghi lại khi thay đổi

// Mắt người nhạy với vùng tối hơn vùng sáng.
// Bình phương mức sáng để LED tắt dần đều mắt.
uint8_t gammaPwm(uint8_t level) {
  return (uint16_t)level * level / 255;
}

// Độ sáng 0..255 tại thời điểm t, tính từ lúc nhóm bắt đầu lượt của mình
uint8_t envelope(unsigned long t) {
  if (ATTACK_MS > 0 && t < ATTACK_MS) {   // đang sáng lên (bỏ qua nếu chớp sáng ngay)
    return t * 255UL / (ATTACK_MS > 0 ? ATTACK_MS : 1);
  }
  t -= ATTACK_MS;
  if (t < DECAY_MS) {                  // đang tắt dần
    return 255 - t * 255UL / DECAY_MS;
  }
  return 0;                            // tắt hẳn, chờ nhóm kia
}

void setup() {
  for (uint8_t i = 0; i < LED_COUNT; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    analogWrite(LED_PINS[i], 0);
    lastPwm[i] = 0;
  }
}

void loop() {
  unsigned long now = millis();

  for (uint8_t i = 0; i < LED_COUNT; i++) {
    // LED 1, 3, 5 (chỉ số 0, 2, 4) chạy trước;
    // LED 2, 4, 6 trễ đúng nửa vòng -> sáng tắt so le
    unsigned long offset = (i % 2 == 0) ? 0 : HALF_MS;
    unsigned long t = (now + PERIOD_MS - offset) % PERIOD_MS;

    int pwm = gammaPwm(envelope(t));
    if (pwm != lastPwm[i]) {
      analogWrite(LED_PINS[i], pwm);
      lastPwm[i] = pwm;
    }
  }
}
