// Chạy thử logic prototype 10: bấm nút (có rung tiếp điểm) từng ms, kiểm tra LED chẵn/lẻ.
// Chạy từ thư mục gốc dự án: bash tests/run_all.sh
#ifdef __AVR__   // Tinkercad, Wokwi, Arduino IDE biên dịch cho chip AVR
#error "Đây là file test chạy trên máy tính, không dán vào mạch. Hãy dán file prototype-10-4-led-chan-le-3-nut/sketch.ino"
#endif
#include "Arduino.h"
#include "../prototype-10-4-led-chan-le-3-nut/sketch.ino"

const int RELEASED = (PRESSED == HIGH) ? LOW : HIGH;
bool ok = true;

void run(unsigned long ms) {
  for (unsigned long i = 0; i < ms; i++) { loop(); fake_ms++; }
}

// Nhấn nút b: rung 5 lần trong 10 ms, giữ holdMs, nhả cũng rung rồi chờ
void press(uint8_t b, unsigned long holdMs) {
  uint8_t p = BUTTON_PINS[b];
  for (int k = 0; k < 5; k++) {
    fake_pin[p] = PRESSED;  run(1);
    fake_pin[p] = RELEASED; run(1);
  }
  fake_pin[p] = PRESSED; run(holdMs);
  for (int k = 0; k < 5; k++) {
    fake_pin[p] = RELEASED; run(1);
    fake_pin[p] = PRESSED;  run(1);
  }
  fake_pin[p] = RELEASED; run(3 * DEBOUNCE_MS);
}

// "C" = chẵn sáng, "L" = lẻ sáng, "T" = tắt hết, "?" = trạng thái lạ
char state() {
  bool chan = true, le = true, tat = true;
  for (uint8_t i = 0; i < LED_COUNT; i++) {
    bool on = fake_pin[LED_PINS[i]] == HIGH;
    bool isChan = (i % 2 == 1);
    if (on != isChan) chan = false;
    if (on == isChan) le = false;
    if (on) tat = false;
  }
  return chan ? 'C' : le ? 'L' : tat ? 'T' : '?';
}

void expect(const char *what, char want) {
  char got = state();
  printf("%-38s -> %c (cần %c)%s\n", what, got, want, got == want ? "" : "  SAI");
  if (got != want) ok = false;
}

int main() {
  for (uint8_t b = 0; b < BUTTON_COUNT; b++) fake_pin[BUTTON_PINS[b]] = RELEASED;
  fake_ms = 0;
  setup();
  run(200);
  expect("Bật nguồn", 'T');
  press(2, 200); expect("Nút 3 khi đang tắt hết", 'C');
  press(2, 200); expect("Nút 3 lần 2 (rung không đảo 2 lần)", 'L');
  press(2, 2000); expect("Nút 3 giữ 2 s (chỉ đảo 1 lần)", 'C');
  press(1, 200); expect("Nút 2", 'L');
  press(1, 200); expect("Nút 2 lần nữa (giữ nguyên)", 'L');
  press(0, 200); expect("Nút 1", 'C');
  press(0, 200); expect("Nút 1 lần nữa (giữ nguyên)", 'C');
  press(2, 200); expect("Nút 3 khi chẵn sáng", 'L');
  press(0, 200); expect("Nút 1 khi lẻ sáng", 'C');

  // Xung nhiễu ngắn hơn thời gian lọc thì bỏ qua
  fake_pin[BUTTON_PINS[1]] = PRESSED;  run(DEBOUNCE_MS / 2);
  fake_pin[BUTTON_PINS[1]] = RELEASED; run(3 * DEBOUNCE_MS);
  expect("Nhiễu nút 2 ngắn hơn DEBOUNCE_MS", 'C');

  printf(ok ? "KẾT QUẢ: ĐẠT\n" : "KẾT QUẢ: KHÔNG ĐẠT\n");
  return ok ? 0 : 1;
}
