// Chạy thử logic prototype 07 trong 2 chu kỳ, mỗi bước 1 ms.
// Chạy từ thư mục gốc dự án: bash tests/run_all.sh
#include "Arduino.h"
#include <stdlib.h>
#include <vector>
#include "../prototype-07-6-led-so-le-chay-tang-dan/sketch.ino"

int main() {
  printf("SHOW_MS=%lu  SO_LE_ROUND=%lu  CHAY_ROUND=%lu\n", SHOW_MS, SO_LE_ROUND_MS, CHAY_ROUND_MS);
  setup();
  const unsigned long T = 2 * SHOW_MS;
  std::vector<std::vector<int> > h(LED_COUNT, std::vector<int>(T + 1));
  for (fake_ms = 0; fake_ms <= T; fake_ms++) {
    loop();
    for (int i = 0; i < LED_COUNT; i++) h[i][fake_ms] = fake_pin[LED_PINS[i]];
  }
  bool ok = true;

  bool dark = true;                                   // tắt hết ngay trước mỗi lần đổi hiệu ứng
  for (unsigned long base = 0; base <= T; base += SHOW_MS) {
    unsigned long sw[2] = {base + SO_LE_TOTAL_MS, base + SHOW_MS};
    for (int k = 0; k < 2; k++)
      if (sw[k] <= T) for (int i = 0; i < LED_COUNT; i++) if (h[i][sw[k] - 1] != 0) dark = false;
  }
  printf("Tắt hết trước khi đổi hiệu ứng: %s\n", dark ? "đúng" : "SAI");
  if (!dark) ok = false;

  bool order = true, full = true;                     // chạy tăng dần: trái sáng trước, đủ dãy thì sáng hết
  int maxStep = 0;
  for (unsigned long t = 1; t <= T; t++) {
    unsigned long s = t % SHOW_MS;
    if (s <= SO_LE_TOTAL_MS) continue;                // chỉ xét pha chạy tăng dần
    for (int i = 1; i < LED_COUNT; i++) if (h[i][t] > h[i - 1][t]) order = false;
    for (int i = 0; i < LED_COUNT; i++) { int d = abs(h[i][t] - h[i][t - 1]); if (d > maxStep) maxStep = d; }
    unsigned long c = (s - SO_LE_TOTAL_MS) % CHAY_ROUND_MS;
    if (c == CHAY_FULL_MS) for (int i = 0; i < LED_COUNT; i++) if (h[i][t] != 255) full = false;
  }
  printf("LED bên trái luôn sáng trước: %s\n", order ? "đúng" : "SAI");
  printf("Cả dãy sáng 255 khi chạy hết: %s\n", full ? "đúng" : "SAI");
  // Gamma bình phương nên PWM đổi nhanh nhất ở gần 255: ≈ 2 × 255 / CHAY_IN_MS mỗi ms, cộng sai số làm tròn
  int limit = 2 * 255 / CHAY_IN_MS + 3;
  printf("PWM đổi tối đa mỗi ms khi chạy tăng dần: %d (cho phép %d)\n", maxStep, limit);
  if (!order || !full || maxStep > limit) ok = false;

  printf(ok ? "KẾT QUẢ: ĐẠT\n" : "KẾT QUẢ: KHÔNG ĐẠT\n");
  return ok ? 0 : 1;
}
