// Chạy thử logic prototype 08 trong 2 chu kỳ, mỗi bước 1 ms.
// Chạy từ thư mục gốc dự án: bash tests/run_all.sh
#include "Arduino.h"
#include "Servo.h"
#include <stdlib.h>
#include <vector>
#include "../prototype-08-5-servo-dong-bo-luon-song/sketch.ino"

int main() {
  printf("SHOW_MS=%lu  DONG_BO_ROUND=%lu  SONG_ROUND=%lu\n", SHOW_MS, DONG_BO_ROUND_MS, SONG_ROUND_MS);
  setup();
  const unsigned long T = 2 * SHOW_MS;
  std::vector<std::vector<int> > h(SERVO_COUNT, std::vector<int>(T + 1));
  for (fake_ms = 0; fake_ms <= T; fake_ms++) {
    loop();
    for (int i = 0; i < SERVO_COUNT; i++) h[i][fake_ms] = servo_angle[i];
  }
  bool ok = true;

  int maxStep = 0;                                    // không có cú nhảy góc
  for (int i = 0; i < SERVO_COUNT; i++)
    for (unsigned long t = 1; t <= T; t++) { int d = abs(h[i][t] - h[i][t - 1]); if (d > maxStep) maxStep = d; }
  printf("Góc đổi tối đa mỗi ms: %d°\n", maxStep);
  if (maxStep > 1) ok = false;

  bool same = true;                                   // pha đồng bộ: 5 servo cùng góc
  for (unsigned long t = 0; t <= T; t++)
    if (t % SHOW_MS < DONG_BO_TOTAL_MS)
      for (int i = 1; i < SERVO_COUNT; i++) if (h[i][t] != h[0][t]) same = false;
  printf("Pha đồng bộ cùng góc: %s\n", same ? "đúng" : "SAI");
  if (!same) ok = false;

  bool zero = true;                                   // mỗi vòng bắt đầu ở 0°
  for (unsigned long base = 0; base <= T; base += SHOW_MS) {
    std::vector<unsigned long> starts;
    for (int r = 0; r < DONG_BO_ROUNDS; r++) starts.push_back(base + r * DONG_BO_ROUND_MS);
    for (int r = 0; r < SONG_ROUNDS; r++) starts.push_back(base + DONG_BO_TOTAL_MS + r * SONG_ROUND_MS);
    for (size_t k = 0; k < starts.size(); k++)
      if (starts[k] <= T) for (int i = 0; i < SERVO_COUNT; i++) if (h[i][starts[k]] != 0) zero = false;
  }
  printf("Mọi vòng bắt đầu ở 0°: %s\n", zero ? "đúng" : "SAI");
  if (!zero) ok = false;

  long minPause = 1L << 30;                           // luôn dừng trước khi đổi chiều
  for (int i = 0; i < SERVO_COUNT; i++) {
    int dir = 0; long still = 0;
    for (unsigned long t = 1; t <= T; t++) {
      int d = h[i][t] - h[i][t - 1];
      if (d == 0) { still++; continue; }
      int nd = d > 0 ? 1 : -1;
      if (dir != 0 && nd != dir && still < minPause) minPause = still;
      dir = nd; still = 0;
    }
  }
  printf("Dừng ít nhất trước khi đổi chiều: %ld ms\n", minPause);
  if (minPause < 400) ok = false;

  printf(ok ? "KẾT QUẢ: ĐẠT\n" : "KẾT QUẢ: KHÔNG ĐẠT\n");
  return ok ? 0 : 1;
}
