// Chạy thử logic prototype 09 trong 2 chu kỳ, mỗi bước 1 ms.
// Chạy từ thư mục gốc dự án: bash tests/run_all.sh
#ifdef __AVR__   // Tinkercad, Wokwi, Arduino IDE biên dịch cho chip AVR
#error "Đây là file test chạy trên máy tính, không dán vào mạch. Hãy dán file prototype-09-5-servo-dong-bo/sketch.ino"
#endif
#include "Arduino.h"
#include "Servo.h"
#include <stdlib.h>
#include <vector>
#include "../prototype-09-5-servo-dong-bo/sketch.ino"

int main() {
  printf("DONG_BO_ROUND=%lu\n", DONG_BO_ROUND_MS);
  setup();
  const unsigned long T = 2 * DONG_BO_ROUND_MS;
  std::vector<std::vector<int> > h(SERVO_COUNT, std::vector<int>(T + 1));
  for (fake_ms = 0; fake_ms <= T; fake_ms++) {
    loop();
    for (int i = 0; i < SERVO_COUNT; i++) h[i][fake_ms] = servo_angle[i];
  }
  bool ok = true;

  int maxStep = 0;                                    // không có cú nhảy góc
  for (int i = 0; i < SERVO_COUNT; i++)
    for (unsigned long t = 1; t <= T; t++) { int d = abs(h[i][t] - h[i][t - 1]); if (d > maxStep) maxStep = d; }
  int allowed = (MAX_ANGLE - MIN_ANGLE + DONG_BO_MOVE_MS - 1) / DONG_BO_MOVE_MS;
  printf("Góc đổi tối đa mỗi ms: %d° (cho phép %d°)\n", maxStep, allowed);
  if (maxStep > allowed) ok = false;

  bool same = true;                                   // 5 servo luôn cùng góc
  for (unsigned long t = 0; t <= T; t++)
    for (int i = 1; i < SERVO_COUNT; i++) if (h[i][t] != h[0][t]) same = false;
  printf("5 servo luôn cùng góc: %s\n", same ? "đúng" : "SAI");
  if (!same) ok = false;

  bool ends = true;                                   // đủ hành trình 0° và 180°
  for (unsigned long base = 0; base + DONG_BO_ROUND_MS <= T; base += DONG_BO_ROUND_MS) {
    if (h[0][base] != MIN_ANGLE) ends = false;
    if (h[0][base + DONG_BO_MOVE_MS] != MAX_ANGLE) ends = false;
    if (h[0][base + DONG_BO_HALF_MS + DONG_BO_MOVE_MS] != MIN_ANGLE) ends = false;
  }
  printf("Mỗi vòng đi đủ 0° -> 180° -> 0°: %s\n", ends ? "đúng" : "SAI");
  if (!ends) ok = false;

  long minPause = 1L << 30;                           // luôn dừng trước khi đổi chiều
  int dir = 0; long still = 0;
  for (unsigned long t = 1; t <= T; t++) {
    int d = h[0][t] - h[0][t - 1];
    if (d == 0) { still++; continue; }
    int nd = d > 0 ? 1 : -1;
    if (dir != 0 && nd != dir && still < minPause) minPause = still;
    dir = nd; still = 0;
  }
  printf("Dừng ít nhất trước khi đổi chiều: %ld ms (cần %lu)\n", minPause, DONG_BO_HOLD_MS);
  if (minPause < (long)DONG_BO_HOLD_MS) ok = false;

  printf(ok ? "KẾT QUẢ: ĐẠT\n" : "KẾT QUẢ: KHÔNG ĐẠT\n");
  return ok ? 0 : 1;
}
