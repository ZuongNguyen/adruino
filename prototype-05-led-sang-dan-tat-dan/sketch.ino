// LED sáng dần rồi tắt dần (fade) – Arduino Uno
// Mạch:
//   LED: chân dương (chân cong) -> chân ~11  (mạch thật: thêm điện trở 220Ω ở giữa)
//        chân âm  (chân thẳng)  -> GND
//   Bắt buộc dùng chân có dấu ~ (PWM): 3, 5, 6, 9, 10, 11

const uint8_t LED_PIN = 11;

// Chỉnh thời gian ở đây (đơn vị: mili giây)
const unsigned long FADE_IN_MS  = 1500;   // thời gian sáng dần
const unsigned long HOLD_ON_MS  = 300;    // giữ sáng nhất
const unsigned long FADE_OUT_MS = 1500;   // thời gian tắt dần
const unsigned long HOLD_OFF_MS = 300;    // giữ tắt hẳn

// 4 pha chạy lần lượt rồi lặp lại
enum Phase { SANG_DAN, GIU_SANG, TAT_DAN, GIU_TAT };
Phase phase = SANG_DAN;
unsigned long phaseStartMs = 0;           // thời điểm bắt đầu pha hiện tại

// Mắt người nhạy với vùng tối hơn vùng sáng.
// Bình phương mức sáng để LED sáng/tắt đều mắt,
// thay vì vọt sáng thật nhanh rồi gần như đứng yên.
uint8_t gammaPwm(uint8_t level) {
  return (uint16_t)level * level / 255;
}

void startPhase(Phase next, unsigned long now) {
  phase = next;
  phaseStartMs = now;
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 0);                // bắt đầu ở trạng thái tắt
  phaseStartMs = millis();
}

void loop() {
  unsigned long now = millis();
  unsigned long t = now - phaseStartMs;   // thời gian đã trôi qua trong pha

  switch (phase) {
    case SANG_DAN:                        // độ sáng tăng theo thời gian: 0 -> 255
      if (t >= FADE_IN_MS) {
        analogWrite(LED_PIN, 255);
        startPhase(GIU_SANG, now);
      } else {
        analogWrite(LED_PIN, gammaPwm(t * 255UL / FADE_IN_MS));
      }
      break;

    case GIU_SANG:
      if (t >= HOLD_ON_MS) startPhase(TAT_DAN, now);
      break;

    case TAT_DAN:                         // độ sáng giảm theo thời gian: 255 -> 0
      if (t >= FADE_OUT_MS) {
        analogWrite(LED_PIN, 0);
        startPhase(GIU_TAT, now);
      } else {
        analogWrite(LED_PIN, gammaPwm(255 - t * 255UL / FADE_OUT_MS));
      }
      break;

    case GIU_TAT:
      if (t >= HOLD_OFF_MS) startPhase(SANG_DAN, now);
      break;
  }
}
