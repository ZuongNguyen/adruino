// Chạy thử logic prototype 15: giai đoạn reset về 0° rồi 2 vòng quét, mỗi bước 1 ms.
// Chạy từ thư mục gốc dự án: bash tests/run_all.sh
#ifdef __AVR__   // Tinkercad, Wokwi, Arduino IDE biên dịch cho chip AVR
#error "Đây là file test chạy trên máy tính, không dán vào mạch. Hãy dán file prototype-15-test-servo-11-10/sketch.ino"
#endif
#include "Arduino.h"
#include "Servo.h"
#include <stdlib.h>
#include <vector>
#include "../prototype-15-test-servo-11-10/sketch.ino"

int main() {
  printf("RESET=%lu QUET_ROUND=%lu\n", RESET_MS, QUET_ROUND_MS);
  bool ok = true;

  servo_angle[0] = 123;                               // servo đang đứng ở một góc bất kỳ
  setup();
  bool resetWrite = servo_angle[0] == MIN_ANGLE;
  printf("setup() đưa servo về 0°: %s\n", resetWrite ? "đúng" : "SAI");
  if (!resetWrite) ok = false;

  const unsigned long T = RESET_MS + 2 * QUET_ROUND_MS;
  std::vector<int> h(T + 1);
  int changes = 0;
  int writesBefore = servo_writes;
  for (fake_ms = 0; fake_ms <= T; fake_ms++) {
    loop();
    h[fake_ms] = servo_angle[0];
    if (fake_ms > 0 && h[fake_ms] != h[fake_ms - 1]) changes++;
  }

  unsigned long firstMove = T + 1;                    // đứng yên ở 0° suốt giai đoạn reset
  for (unsigned long t = 0; t <= T; t++) if (h[t] != MIN_ANGLE) { firstMove = t; break; }
  printf("Đứng ở 0° trước khi quay lần đầu: %lu ms (cần %lu)\n", firstMove, RESET_MS);
  if (firstMove < RESET_MS || firstMove > T) ok = false;

  int maxStep = 0;                                    // không có cú nhảy góc
  for (unsigned long t = 1; t <= T; t++) { int d = abs(h[t] - h[t - 1]); if (d > maxStep) maxStep = d; }
  int allowed = (MAX_ANGLE - MIN_ANGLE + QUET_MOVE_MS - 1) / QUET_MOVE_MS;
  printf("Góc đổi tối đa mỗi ms: %d° (cho phép %d°)\n", maxStep, allowed);
  if (maxStep > allowed) ok = false;

  bool ends = true;                                   // mỗi vòng đi đủ 0° -> 180° -> 0°
  for (unsigned long base = RESET_MS; base + QUET_ROUND_MS <= T; base += QUET_ROUND_MS) {
    if (h[base] != MIN_ANGLE) ends = false;
    if (h[base + QUET_MOVE_MS] != MAX_ANGLE) ends = false;
    if (h[base + QUET_HALF_MS + QUET_MOVE_MS] != MIN_ANGLE) ends = false;
    if (h[base + QUET_ROUND_MS] != MIN_ANGLE) ends = false;
  }
  printf("Mỗi vòng đi đủ 0° -> 180° -> 0°: %s\n", ends ? "đúng" : "SAI");
  if (!ends) ok = false;

  long minPause = 1L << 30;                           // luôn dừng trước khi đổi chiều
  int dir = 0; long still = 0;
  for (unsigned long t = 1; t <= T; t++) {
    int d = h[t] - h[t - 1];
    if (d == 0) { still++; continue; }
    int nd = d > 0 ? 1 : -1;
    if (dir != 0 && nd != dir && still < minPause) minPause = still;
    dir = nd; still = 0;
  }
  printf("Dừng ít nhất trước khi đổi chiều: %ld ms (cần %lu)\n", minPause, QUET_HOLD_MS);
  if (minPause < (long)QUET_HOLD_MS) ok = false;

  int writes = servo_writes - writesBefore;           // chỉ ghi servo khi góc đổi
  printf("Số lần ghi servo trong loop(): %d (góc đổi %d lần)\n", writes, changes);
  if (writes != changes) ok = false;

  printf(ok ? "KẾT QUẢ: ĐẠT\n" : "KẾT QUẢ: KHÔNG ĐẠT\n");
  return ok ? 0 : 1;
}
