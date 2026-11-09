// Chạy thử logic prototype 13 (2 nút bật/tắt 2 LED): bấm nút (có rung tiếp điểm) từng ms, kiểm tra LED.
// Chạy từ thư mục gốc dự án: bash tests/run_all.sh
#ifdef __AVR__   // Tinkercad, Wokwi, Arduino IDE biên dịch cho chip AVR
#error "Đây là file test chạy trên máy tính, không dán vào mạch. Hãy dán file prototype-13-2-nut-bat-tat-2-led/sketch.ino"
#endif
#include "Arduino.h"
#include "../prototype-13-2-nut-bat-tat-2-led/sketch.ino"

bool ok = true;

int released(uint8_t b) { return PRESSED[b] == HIGH ? LOW : HIGH; }

void run(unsigned long ms) {
  for (unsigned long i = 0; i < ms; i++) { loop(); fake_ms++; }
}

// Nhấn các nút có bit trong mask cùng lúc: rung 5 lần trong 10 ms, giữ holdMs, nhả cũng rung rồi chờ
void press(uint8_t mask, unsigned long holdMs) {
  for (int k = 0; k < 5; k++) {
    for (uint8_t b = 0; b < BUTTON_COUNT; b++) if (mask & (1 << b)) fake_pin[BUTTON_PINS[b]] = PRESSED[b];
    run(1);
    for (uint8_t b = 0; b < BUTTON_COUNT; b++) if (mask & (1 << b)) fake_pin[BUTTON_PINS[b]] = released(b);
    run(1);
  }
  for (uint8_t b = 0; b < BUTTON_COUNT; b++) if (mask & (1 << b)) fake_pin[BUTTON_PINS[b]] = PRESSED[b];
  run(holdMs);
  for (int k = 0; k < 5; k++) {
    for (uint8_t b = 0; b < BUTTON_COUNT; b++) if (mask & (1 << b)) fake_pin[BUTTON_PINS[b]] = released(b);
    run(1);
    for (uint8_t b = 0; b < BUTTON_COUNT; b++) if (mask & (1 << b)) fake_pin[BUTTON_PINS[b]] = PRESSED[b];
    run(1);
  }
  for (uint8_t b = 0; b < BUTTON_COUNT; b++) if (mask & (1 << b)) fake_pin[BUTTON_PINS[b]] = released(b);
  run(3 * DEBOUNCE_MS);
}

// want: chuỗi mỗi LED một ký tự, '1' = sáng, '0' = tắt (LED đỏ trước, LED xanh sau)
void expect(const char *what, const char *want) {
  char got[8];
  for (uint8_t b = 0; b < BUTTON_COUNT; b++) got[b] = fake_pin[LED_PINS[b]] == HIGH ? '1' : '0';
  got[BUTTON_COUNT] = 0;
  bool same = strcmp(got, want) == 0;
  printf("%-40s -> đỏ/xanh %s (cần %s)%s\n", what, got, want, same ? "" : "  SAI");
  if (!same) ok = false;
}

int main() {
  for (uint8_t b = 0; b < BUTTON_COUNT; b++) fake_pin[BUTTON_PINS[b]] = released(b);
  fake_ms = 0;
  setup();
  run(200);
  expect("Bật nguồn", "00");
  press(1, 200); expect("Nút 1", "10");
  press(1, 200); expect("Nút 1 lần 2", "00");
  press(2, 200); expect("Nút 2 (LED đỏ giữ nguyên)", "01");
  press(1, 200); expect("Nút 1 khi LED xanh đang sáng", "11");
  press(2, 2000); expect("Nút 2 giữ 2 s (chỉ đổi 1 lần)", "10");
  press(3, 200); expect("Nhấn cả 2 nút cùng lúc", "01");
  press(3, 200); expect("Nhấn cả 2 nút lần nữa", "10");

  // Xung nhiễu ngắn hơn thời gian lọc thì bỏ qua
  for (uint8_t b = 0; b < BUTTON_COUNT; b++) {
    fake_pin[BUTTON_PINS[b]] = PRESSED[b];  run(DEBOUNCE_MS / 2);
    fake_pin[BUTTON_PINS[b]] = released(b); run(3 * DEBOUNCE_MS);
  }
  expect("Nhiễu ngắn hơn DEBOUNCE_MS trên 2 nút", "10");

  printf(ok ? "KẾT QUẢ: ĐẠT\n" : "KẾT QUẢ: KHÔNG ĐẠT\n");
  return ok ? 0 : 1;
}
