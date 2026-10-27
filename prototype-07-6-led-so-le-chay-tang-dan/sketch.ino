// 6 LED: chớp so le + chạy tăng dần từ trái qua phải – Arduino Uno
// Mạch giữ nguyên như prototype 06 (LED xếp từ trái sang phải):
//   LED 1 -> ~11   LED 2 -> ~10   LED 3 -> ~9
//   LED 4 -> ~6    LED 5 -> ~5    LED 6 -> ~3
//   Chân dương (chân cong) nối chân PWM, chân âm nối hàng GND trên breadboard
//   Mạch thật: thêm điện trở 220Ω nối tiếp mỗi LED
//
// Chương trình tự chuyển qua lại giữa 2 hiệu ứng:
//   1. So le:         LED lẻ (1, 3, 5) và chẵn (2, 4, 6) thay phiên chớp sáng rồi tắt dần
//   2. Chạy tăng dần: LED lần lượt sáng dần từ trái qua phải đến khi sáng hết dãy,
//                     giữ một nhịp rồi cả dãy cùng tắt dần

const uint8_t LED_PINS[] = {11, 10, 9, 6, 5, 3};   // thứ tự từ trái qua phải
const uint8_t LED_COUNT  = sizeof(LED_PINS) / sizeof(LED_PINS[0]);

// ---- Số vòng chạy mỗi hiệu ứng trước khi chuyển (đặt 0 để bỏ hiệu ứng đó) ----
const uint8_t SO_LE_ROUNDS = 3;
const uint8_t CHAY_ROUNDS  = 2;

// ---- Hiệu ứng 1: so le (đơn vị: mili giây) ----
const unsigned long SO_LE_ATTACK_MS = 0;     // 0 = chớp sáng ngay, >0 = sáng dần
const unsigned long SO_LE_DECAY_MS  = 700;   // thời gian tắt dần
const unsigned long SO_LE_GAP_MS    = 100;   // tắt hẳn trước khi nhóm kia chớp
const unsigned long SO_LE_HALF_MS   = SO_LE_ATTACK_MS + SO_LE_DECAY_MS + SO_LE_GAP_MS;
const unsigned long SO_LE_ROUND_MS  = 2 * SO_LE_HALF_MS;

// ---- Hiệu ứng 2: chạy tăng dần từ trái qua phải (đơn vị: mili giây) ----
const unsigned long CHAY_STEP_MS = 250;      // LED kế bên phải bắt đầu sáng sau bao lâu
const unsigned long CHAY_IN_MS   = 250;      // mỗi LED sáng dần lên trong bao lâu
const unsigned long CHAY_HOLD_MS = 500;      // giữ khi cả dãy đã sáng hết
const unsigned long CHAY_OUT_MS  = 600;      // cả dãy cùng tắt dần
const unsigned long CHAY_GAP_MS  = 200;      // tắt hẳn trước vòng tiếp theo
const unsigned long CHAY_FULL_MS  = (LED_COUNT - 1) * CHAY_STEP_MS + CHAY_IN_MS;  // lúc LED cuối sáng hết
const unsigned long CHAY_ROUND_MS = CHAY_FULL_MS + CHAY_HOLD_MS + CHAY_OUT_MS + CHAY_GAP_MS;

// ---- Cả chương trình: so le N vòng -> chạy tăng dần M vòng -> lặp lại ----
const unsigned long SO_LE_TOTAL_MS = SO_LE_ROUNDS * SO_LE_ROUND_MS;
const unsigned long SHOW_MS        = SO_LE_TOTAL_MS + CHAY_ROUNDS * CHAY_ROUND_MS;
static_assert(SHOW_MS > 0, "Can it nhat mot hieu ung co so vong lon hon 0");

int lastPwm[LED_COUNT];                      // giá trị đã ghi, chỉ ghi lại khi thay đổi

// Mắt người nhạy với vùng tối hơn vùng sáng.
// Bình phương mức sáng để LED sáng/tắt đều mắt.
uint8_t gammaPwm(uint8_t level) {
  return (uint16_t)level * level / 255;
}

// Tăng từ 0 lên 255 trong khoảng `duration`, tính tại thời điểm t
uint8_t rampUp(unsigned long t, unsigned long duration) {
  if (duration == 0 || t >= duration) return 255;
  return t * 255UL / duration;
}

// Hiệu ứng 1: độ sáng LED thứ i tại thời điểm t trong một vòng so le
uint8_t levelSoLe(uint8_t i, unsigned long t) {
  unsigned long offset = (i % 2 == 0) ? 0 : SO_LE_HALF_MS;   // nhóm chẵn trễ đúng nửa vòng
  t = (t + SO_LE_ROUND_MS - offset) % SO_LE_ROUND_MS;
  if (t < SO_LE_ATTACK_MS) return rampUp(t, SO_LE_ATTACK_MS);           // đang sáng lên
  t -= SO_LE_ATTACK_MS;
  if (t < SO_LE_DECAY_MS) return 255 - rampUp(t, SO_LE_DECAY_MS);      // đang tắt dần
  return 0;                                                              // chờ nhóm kia
}

// Hiệu ứng 2: độ sáng LED thứ i tại thời điểm t trong một vòng chạy tăng dần
uint8_t levelChay(uint8_t i, unsigned long t) {
  unsigned long start = i * CHAY_STEP_MS;    // LED càng bên phải càng sáng muộn
  if (t < CHAY_FULL_MS + CHAY_HOLD_MS) {     // giai đoạn sáng tăng dần + giữ
    return (t < start) ? 0 : rampUp(t - start, CHAY_IN_MS);
  }
  t -= CHAY_FULL_MS + CHAY_HOLD_MS;
  if (t < CHAY_OUT_MS) return 255 - rampUp(t, CHAY_OUT_MS);             // cả dãy tắt dần
  return 0;                                                              // nghỉ trước vòng sau
}

void setup() {
  for (uint8_t i = 0; i < LED_COUNT; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    analogWrite(LED_PINS[i], 0);
    lastPwm[i] = 0;
  }
}

void loop() {
  unsigned long tShow = millis() % SHOW_MS;  // vị trí hiện tại trong cả chương trình

  for (uint8_t i = 0; i < LED_COUNT; i++) {
    uint8_t level;
    if (tShow < SO_LE_TOTAL_MS) {
      level = levelSoLe(i, tShow % SO_LE_ROUND_MS);
    } else {
      level = levelChay(i, (tShow - SO_LE_TOTAL_MS) % CHAY_ROUND_MS);
    }

    int pwm = gammaPwm(level);
    if (pwm != lastPwm[i]) {
      analogWrite(LED_PINS[i], pwm);
      lastPwm[i] = pwm;
    }
  }
}
