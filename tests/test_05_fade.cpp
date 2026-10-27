// Chạy thử logic prototype 05 trong 2 chu kỳ, mỗi bước 1 ms.
// Chạy từ thư mục gốc dự án: bash tests/run_all.sh
#include "Arduino.h"
#include <stdlib.h>
#include <vector>
#include "../prototype-05-led-sang-dan-tat-dan/sketch.ino"

int main() {
  const unsigned long PERIOD = FADE_IN_MS + HOLD_ON_MS + FADE_OUT_MS + HOLD_OFF_MS;
  printf("PERIOD=%lu\n", PERIOD);
  setup();
  const unsigned long T = 2 * PERIOD;
  std::vector<int> h(T + 1);
  for (fake_ms = 0; fake_ms <= T; fake_ms++) {
    loop();
    h[fake_ms] = fake_pin[LED_PIN];
  }
  bool ok = true;

  int maxStep = 0, lo = 255, hi = 0;                  // sáng/tắt dần, không nhảy
  for (unsigned long t = 0; t <= T; t++) {
    if (h[t] < lo) lo = h[t];
    if (h[t] > hi) hi = h[t];
    if (t > 0) { int d = abs(h[t] - h[t - 1]); if (d > maxStep) maxStep = d; }
  }
  printf("Độ sáng thấp nhất/cao nhất: %d / %d\n", lo, hi);
  printf("PWM đổi tối đa mỗi ms: %d\n", maxStep);
  if (lo != 0 || hi != 255 || maxStep > 2) ok = false;

  bool hold = true;                                   // giữ sáng và giữ tắt đúng thời gian
  for (unsigned long base = 0; base + PERIOD <= T; base += PERIOD) {
    for (unsigned long t = FADE_IN_MS + 1; t < FADE_IN_MS + HOLD_ON_MS; t++)
      if (h[base + t] != 255) hold = false;
    for (unsigned long t = PERIOD - HOLD_OFF_MS + 1; t < PERIOD; t++)
      if (h[base + t] != 0) hold = false;
  }
  printf("Giữ sáng / giữ tắt đúng: %s\n", hold ? "đúng" : "SAI");
  if (!hold) ok = false;

  printf(ok ? "KẾT QUẢ: ĐẠT\n" : "KẾT QUẢ: KHÔNG ĐẠT\n");
  return ok ? 0 : 1;
}
