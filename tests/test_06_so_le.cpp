// Chạy thử logic prototype 06 trong 2 vòng, mỗi bước 1 ms.
// Chạy từ thư mục gốc dự án: bash tests/run_all.sh
#include "Arduino.h"
#include <vector>
#include "../prototype-06-6-led-chop-so-le/sketch.ino"

int main() {
  printf("PERIOD_MS=%lu  HALF_MS=%lu\n", PERIOD_MS, HALF_MS);
  setup();
  const unsigned long T = 2 * PERIOD_MS;
  std::vector<std::vector<int> > h(LED_COUNT, std::vector<int>(T + 1));
  for (fake_ms = 0; fake_ms <= T; fake_ms++) {
    loop();
    for (int i = 0; i < LED_COUNT; i++) h[i][fake_ms] = fake_pin[LED_PINS[i]];
  }
  bool ok = true;

  bool group = true, apart = true;                    // cùng nhóm cùng sáng, hai nhóm không chồng nhau
  for (unsigned long t = 0; t <= T; t++) {
    for (int i = 2; i < LED_COUNT; i++) if (h[i][t] != h[i - 2][t]) group = false;
    if (h[0][t] > 0 && h[1][t] > 0) apart = false;
  }
  printf("LED cùng nhóm sáng như nhau: %s\n", group ? "đúng" : "SAI");
  printf("Nhóm lẻ và chẵn không sáng cùng lúc: %s\n", apart ? "đúng" : "SAI");
  if (!group || !apart) ok = false;

  bool flash = true;                                  // mỗi nhóm chớp sáng hết cỡ đúng lượt của mình
  for (unsigned long base = 0; base < T; base += PERIOD_MS) {
    if (h[0][base + ATTACK_MS] != 255) flash = false;
    if (h[1][base + HALF_MS + ATTACK_MS] != 255) flash = false;
  }
  printf("Mỗi nhóm sáng 255 đúng lượt: %s\n", flash ? "đúng" : "SAI");
  if (!flash) ok = false;

  printf(ok ? "KẾT QUẢ: ĐẠT\n" : "KẾT QUẢ: KHÔNG ĐẠT\n");
  return ok ? 0 : 1;
}
