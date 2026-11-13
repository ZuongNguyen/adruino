// Kiểm tra bản 16 từng ms: reset, quay lần lượt, đứng yên và tràn millis().
// Các biến thể được run_all.sh sinh trong build/, không sửa sketch gốc.
#ifdef __AVR__
#error "Đây là test trên máy tính, không nạp lên Arduino."
#endif
#include "Arduino.h"
#include "Servo.h"
#include <limits.h>
#ifdef TEST_16_ZERO_GAP
#include "../build/test_16_zero_gap.inc"
#elif defined(TEST_16_ZERO_MOVE)
#include "../build/test_16_zero_move.inc"
#else
#include "../prototype-16-reset-5-servo/sketch.ino"
#endif

unsigned long moveBegin(unsigned int k) {
  return (SERVO_COUNT - 1UL) * RESET_GAP_MS + RESET_HOLD_MS +
         k * (MOVE_MS + MOVE_PAUSE_MS);
}

// Tính độc lập từ lịch đề bài, không gọi moveAngleAt() của sketch.
int expectedAngle(int servo, unsigned long elapsed) {
  for (unsigned int k = 0; k < MOVE_COUNT; k++) {
    if (MOVE_SERVOS[k] != servo + 1) continue;
    const unsigned long begin = moveBegin(k);
    if (elapsed < begin) return RESET_ANGLE;
    if (MOVE_MS == 0 || elapsed >= begin + MOVE_MS) return MOVE_ANGLE;
    const unsigned long divisor = MOVE_MS == 0 ? 1 : MOVE_MS;
    return RESET_ANGLE + (uint64_t)(MOVE_ANGLE - RESET_ANGLE) *
                         (elapsed - begin) / divisor;
  }
  return RESET_ANGLE;
}

bool runStartup(uint32_t start, const char *name) {
  // Mô phỏng MCU khởi động mới: thay Servo và khởi tạo lại cờ toàn cục.
  // Ba ca dùng 15/16 slot mock; mỗi đối tượng chỉ chạy setup() một lần.
  const int base = servo_attaches;
  if (base + SERVO_COUNT > 16) {
    printf("KHÔNG ĐẠT: số lần attach của ca trước vượt sức chứa mock\n");
    return false;
  }
  const int writesBefore = servo_writes;
  for (int i = 0; i < SERVO_COUNT; i++) servos[i] = Servo();
  xong = false;
  fake_ms = start;
  setup();
  bool startup = servo_attaches == base && servo_writes == writesBefore + SERVO_COUNT;
  for (int i = 0; i < SERVO_COUNT; i++) {
    startup = startup && !servos[i].attached() && servo_angle[base + i] == -123 &&
              servo_trace[base + i].attach_calls == 0 &&
              servo_trace[base + i].output_calls == 0;
  }

  const unsigned long finalEnd = moveBegin(MOVE_COUNT - 1) + MOVE_MS;
  // Chạy một lần: theo dõi ít nhất hai lần toàn bộ lịch và ít nhất 10 s.
  const unsigned long duration = 2 * finalEnd > 10000UL ? 2 * finalEnd : 10000UL;
  const int span = MOVE_ANGLE - RESET_ANGLE;
  const int allowedStep = MOVE_MS == 0 ? span : (span + MOVE_MS - 1) / MOVE_MS;
  int previousAngle[SERVO_COUNT], previousExpected[SERVO_COUNT];
  int expectedOutputs[SERVO_COUNT] = {};
  bool expectedAttached[SERVO_COUNT] = {};
  for (int i = 0; i < SERVO_COUNT; i++) {
    previousAngle[i] = previousExpected[i] = RESET_ANGLE;
  }
  int expectedWrites = SERVO_COUNT, maxStep = 0;
  bool schedule = true, angles = true, smooth = true, sequential = true;
  bool onlyChanges = true, stoppedWrites = true, finished = true;
  unsigned long firstFailure = duration + 1, firstFinishFailure = duration + 1;
  unsigned long actualFinish = duration + 1, wrapAt = duration + 1;
  uint32_t previousMs = start;

  for (unsigned long elapsed = 0; elapsed <= duration; elapsed++) {
    fake_ms = (uint32_t)(start + elapsed);
    if (elapsed != 0 && fake_ms < previousMs) wrapAt = elapsed;
    previousMs = fake_ms;
    const int beforeLoopWrites = servo_writes;
    loop();
    if (xong && actualFinish > duration) actualFinish = elapsed;
    if (xong != (elapsed >= finalEnd)) {
      finished = false;
      if (firstFinishFailure > duration) firstFinishFailure = elapsed;
    }

    int expectedCount = 0, moving = 0, changed = 0;
    for (int i = 0; i < SERVO_COUNT; i++) {
      const int slot = base + i;
      const unsigned long deadline = (unsigned long)i * RESET_GAP_MS;
      const bool due = elapsed >= deadline;
      const int want = expectedAngle(i, elapsed);
      if (due) {
        expectedCount++;
        if (!expectedAttached[i]) {
          expectedAttached[i] = true;
          expectedOutputs[i]++;  // attach áp góc 0° đã ghi trước đó.
        }
        if (want != previousExpected[i]) {
          expectedWrites++;
          expectedOutputs[i]++;
        }
      }
      previousExpected[i] = want;
      schedule = schedule && servos[i].attached() == due;
      const ServoMockTrace &trace = servo_trace[slot];
      if (due) {
        schedule = schedule && trace.pin == SERVO_PINS[i] &&
                   trace.first_attach_ms == (uint32_t)(start + deadline) &&
                   trace.attach_calls == 1;
        angles = angles && trace.first_angle == RESET_ANGLE &&
                 servo_angle[slot] == want && trace.min_angle == RESET_ANGLE &&
                 trace.max_angle <= MOVE_ANGLE;
        const int step = servo_angle[slot] - previousAngle[i];
        if (step > maxStep) maxStep = step;
        smooth = smooth && step >= 0 && step <= allowedStep;
        if (step != 0) changed++;
        if (servo_angle[slot] > RESET_ANGLE && servo_angle[slot] < MOVE_ANGLE) moving++;
        previousAngle[i] = servo_angle[slot];
      } else {
        schedule = schedule && trace.attach_calls == 0 && trace.output_calls == 0 &&
                   servo_angle[slot] == -123;
      }
      // Đếm cả góc thoáng qua trong một loop(), không chỉ đầu ra cuối.
      onlyChanges = onlyChanges && trace.output_calls == expectedOutputs[i];
    }
    schedule = schedule && servo_attaches == base + expectedCount;
    onlyChanges = onlyChanges && servo_writes == writesBefore + expectedWrites;
    sequential = sequential && moving <= 1 && changed <= 1;
    if (elapsed > finalEnd) stoppedWrites = stoppedWrites && servo_writes == beforeLoopWrites;

    // Gọi lại cùng ms: không attach/write lại, kể cả đúng điểm đổi pha.
    const int beforeRepeat = servo_writes;
    loop();
    onlyChanges = onlyChanges && servo_writes == beforeRepeat;
    schedule = schedule && servo_attaches == base + expectedCount;
    for (int i = 0; i < SERVO_COUNT; i++) {
      onlyChanges = onlyChanges && servo_trace[base + i].output_calls == expectedOutputs[i];
    }
    if (!(startup && schedule && angles && smooth && sequential && onlyChanges && stoppedWrites) &&
        firstFailure > duration) firstFailure = elapsed;
  }

  // Sau khi xong, mốc millis rất xa/kế bên tràn không được phát lại lệnh.
  const int idleWrites = servo_writes;
  const uint32_t idleTimes[] = {UINT32_MAX, 0, 1};
  for (unsigned int j = 0; j < sizeof(idleTimes) / sizeof(idleTimes[0]); j++) {
    fake_ms = idleTimes[j];
    loop();
    stoppedWrites = stoppedWrites && xong && servo_writes == idleWrites &&
                    servo_attaches == base + SERVO_COUNT;
    for (int i = 0; i < SERVO_COUNT; i++) {
      stoppedWrites = stoppedWrites && servo_angle[base + i] == expectedAngle(i, finalEnd) &&
                      servo_trace[base + i].output_calls == expectedOutputs[i];
    }
  }
  const bool crossedWrap = start == 0 || wrapAt <= duration;
  printf("%s: start=%lu, GAP=%lu, MOVE=%lu, theo dõi %lu ms (bước 1 ms)\n",
         name, (unsigned long)start, RESET_GAP_MS, MOVE_MS, duration);
  printf("  setup chưa phát đầu ra: %s; reset đúng chân/thứ tự/thời điểm: %s\n",
         startup ? "đúng" : "SAI", schedule ? "đúng" : "SAI");
  printf("  Lịch góc và servo 1/4/5 giữ 0°: %s; tăng đơn điệu, bước tối đa %d° (cho phép %d°): %s\n",
         angles ? "đúng" : "SAI", maxStep, allowedStep, smooth ? "đúng" : "SAI");
  printf("  Quay lần lượt: %s; chỉ ghi khi đổi: %s; không ghi sau %lu ms: %s\n",
         sequential ? "đúng" : "SAI", onlyChanges ? "đúng" : "SAI", finalEnd,
         stoppedWrites ? "đúng" : "SAI");
  printf("  Cờ xong phải bật tại %lu ms: %s (thực tế %lu ms)\n",
         finalEnd, finished ? "đúng" : "SAI", actualFinish);
  if (wrapAt <= duration) printf("  millis() tràn tại elapsed=%lu ms, tiếp tục kiểm từng ms\n", wrapAt);
  if (firstFailure <= duration) printf("  Sai đầu ra đầu tiên tại elapsed=%lu ms\n", firstFailure);
  if (firstFinishFailure <= duration) printf("  Sai cờ xong đầu tiên tại elapsed=%lu ms\n", firstFinishFailure);
  return startup && schedule && angles && smooth && sequential && onlyChanges &&
         stoppedWrites && finished && crossedWrap;
}

int main() {
  const uint8_t expectedPins[] = {5, 6, 9, 10, 11};
  const uint8_t expectedMovers[] = {2, 3};
  if (ULONG_MAX != UINT32_MAX || SERVO_COUNT != sizeof(expectedPins) || RESET_ANGLE != 0 ||
      MOVE_COUNT != sizeof(expectedMovers) || MOVE_ANGLE != 45 ||
      RESET_HOLD_MS != 500 || MOVE_PAUSE_MS != 500) {
    printf("KẾT QUẢ: KHÔNG ĐẠT (sai cấu hình đề bài hoặc millis không phải 32 bit)\n");
    return 1;
  }
  for (int i = 0; i < SERVO_COUNT; i++) {
    if (SERVO_PINS[i] != expectedPins[i]) {
      printf("KẾT QUẢ: KHÔNG ĐẠT (sai chân servo %d)\n", i + 1);
      return 1;
    }
  }
  for (int k = 0; k < MOVE_COUNT; k++) {
    if (MOVE_SERVOS[k] != expectedMovers[k]) {
      printf("KẾT QUẢ: KHÔNG ĐẠT (sai số/thứ tự servo quay)\n");
      return 1;
    }
  }
  // Khóa cấu hình đề bài; mốc/giới hạn ở trên vẫn tính từ hằng sketch.
#ifdef TEST_16_ZERO_GAP
  const unsigned long expectedGap = 0;
#else
  const unsigned long expectedGap = 500;
#endif
#ifdef TEST_16_ZERO_MOVE
  const unsigned long expectedMove = 0;
#else
  const unsigned long expectedMove = 3000;
#endif
  if (RESET_GAP_MS != expectedGap || MOVE_MS != expectedMove) {
    printf("KẾT QUẢ: KHÔNG ĐẠT (sai thời gian mặc định/biến thể)\n");
    return 1;
  }
  printf("Lịch quay: servo 2 %lu..%lu ms; servo 3 %lu..%lu ms\n",
         moveBegin(0), moveBegin(0) + MOVE_MS, moveBegin(1), moveBegin(1) + MOVE_MS);
  for (int i = 0; i < 16; i++) servo_angle[i] = -123;
  const bool normal = runStartup(0, "Khởi động thường");
  const unsigned long resetWrap = RESET_GAP_MS != 0 ? RESET_GAP_MS / 2 : RESET_HOLD_MS / 2;
  const bool resetRollover = runStartup((uint32_t)(0UL - resetWrap), "Tràn trong bước reset/chờ");
  const unsigned long moveWrap = moveBegin(0) + MOVE_MS / 2;
  const bool moveRollover = runStartup((uint32_t)(0UL - moveWrap), "Tràn trong bước quay");
  const bool ok = normal && resetRollover && moveRollover;
  printf(ok ? "KẾT QUẢ: ĐẠT\n" : "KẾT QUẢ: KHÔNG ĐẠT\n");
  return ok ? 0 : 1;
}
