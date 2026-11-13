// Kiểm tra prototype 15 trong 2 chu kỳ quanh lúc millis() 32 bit tràn về 0.
// Chạy từ thư mục gốc dự án: bash tests/run_all.sh
#ifdef __AVR__
#error "Đây là file test trên máy tính, không dán vào mạch. Hãy dùng sketch của bản 15."
#endif
#include "Arduino.h"
#include "Servo.h"
#include <stdlib.h>
#include <vector>
#include "../prototype-15-test-servo-11-10/sketch.ino"

int main() {
  if (QUET_MOVE_MS == 0 || QUET_ROUND_MS == 0) {
    printf("KẾT QUẢ: KHÔNG ĐẠT (thời gian quét phải lớn hơn 0)\n");
    return 1;
  }

  fake_ms = 0;
  setup();
  // Ghi một vòng bình thường làm mẫu: sau tràn phải tiếp tục đúng pha
  // của vòng này, không chỉ đứng ở một góc hợp lệ mà không có cú nhảy.
  std::vector<int> reference(QUET_ROUND_MS);
  for (fake_ms = 0; fake_ms < RESET_MS + QUET_ROUND_MS; fake_ms++) {
    loop();
    if (fake_ms >= RESET_MS) reference[fake_ms - RESET_MS] = servo_angle[0];
  }
  const uint64_t wrap = (uint64_t)UINT32_MAX + 1;
  const uint64_t start = wrap - QUET_ROUND_MS;
  const uint64_t end = wrap + QUET_ROUND_MS;
  const int allowed = (MAX_ANGLE - MIN_ANGLE + QUET_MOVE_MS - 1) / QUET_MOVE_MS;
  int previous = MIN_ANGLE;
  int maxStep = 0;
  int beforeWrap = MIN_ANGLE, afterWrap = MIN_ANGLE;
  int changes = 0, writesBefore = 0;
  bool inRange = true;
  bool samePhase = true;
  uint64_t firstMismatch = 0;
  int expectedAtMismatch = MIN_ANGLE, actualAtMismatch = MIN_ANGLE;

  for (uint64_t elapsed = start; elapsed <= end; elapsed++) {
    // Ép millis() về 32 bit như Uno (bộ kiểm tra dùng g++ WinLibs trên Windows).
    fake_ms = (uint32_t)elapsed;
    loop();
    const int angle = servo_angle[0];
    if (angle < MIN_ANGLE || angle > MAX_ANGLE) inRange = false;
    const int expected = reference[(elapsed - RESET_MS) % QUET_ROUND_MS];
    if (angle != expected && samePhase) {
      samePhase = false;
      firstMismatch = elapsed;
      expectedAtMismatch = expected;
      actualAtMismatch = angle;
    }
    if (elapsed == start) {
      // Bỏ qua bước nhảy thời gian từ setup tới đầu cửa sổ kiểm tra.
      writesBefore = servo_writes;
    } else {
      const int step = abs(angle - previous);
      if (step > maxStep) maxStep = step;
      if (step != 0) changes++;
    }
    if (elapsed == wrap - 1) beforeWrap = angle;
    if (elapsed == wrap) afterWrap = angle;
    previous = angle;
  }

  const int writes = servo_writes - writesBefore;
  printf("Hai chu kỳ quanh millis() tràn: %llu ms, mỗi bước 1 ms\n",
         (unsigned long long)(end - start));
  printf("millis() %lu -> 0: servo %d° -> %d°\n",
         (unsigned long)UINT32_MAX, beforeWrap, afterWrap);
  printf("Góc đổi tối đa mỗi ms: %d° (cho phép %d°)\n", maxStep, allowed);
  printf("Góc trong giới hạn: %s; số lần ghi/góc đổi: %d/%d\n",
         inRange ? "đúng" : "SAI", writes, changes);
  printf("Giữ đúng pha quét và khoảng dừng qua millis() tràn: %s\n",
         samePhase ? "đúng" : "SAI");
  if (!samePhase) {
    printf("Sai đầu tiên tại %llu ms: góc %d°, cần %d°\n",
           (unsigned long long)firstMismatch, actualAtMismatch, expectedAtMismatch);
  }
  const bool ok = inRange && samePhase && maxStep <= allowed && writes == changes;
  printf(ok ? "KẾT QUẢ: ĐẠT\n" : "KẾT QUẢ: KHÔNG ĐẠT\n");
  return ok ? 0 : 1;
}
