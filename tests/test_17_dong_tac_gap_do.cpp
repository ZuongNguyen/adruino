// Kiểm tra bản 17 từng ms: đặt góc đầu, 7 lượt quay, nghỉ, dừng và tràn millis().
// Biến thể chỉ được sinh trong build/ bởi run_all.sh, không sửa sketch gốc.
#ifdef __AVR__
#error "Test chạy trên máy tính, không nạp lên Arduino."
#endif
#include "Arduino.h"
#include "Servo.h"
#include <limits.h>
#include <stdlib.h>
#include <vector>
#ifdef TEST_17_ZERO_BOTH
#include "../build/test_17_zero_both.inc"
#elif defined(TEST_17_ZERO_GAP)
#include "../build/test_17_zero_gap.inc"
#elif defined(TEST_17_ZERO_SPEED)
#include "../build/test_17_zero_speed.inc"
#else
#include "../prototype-17-dong-tac-gap-do/sketch.ino"
#endif

struct ExpectedMove {
  int servo, from, to;
  unsigned long begin, end;
};
std::vector<ExpectedMove> moves;

// Oracle dùng số học 64 bit và trạng thái riêng; không gọi buocAngleAt(),
// không dùng buocTu/buocBatDau/buocThoiGian do setup() của sketch lập.
void makeExpectedMoves() {
  int position[SERVO_COUNT];
  for (int i = 0; i < SERVO_COUNT; i++) position[i] = GOC_BAN_DAU[i];
  uint64_t cursor = (SERVO_COUNT - 1ULL) * SET_GAP_MS + SET_HOLD_MS;
  for (int k = 0; k < BUOC_COUNT; k++) {
    const int servo = BUOC_1[k].servo - 1;
    if (k != 0) cursor += BUOC_1[k - 1].servo == BUOC_1[k].servo ?
                         NGHI_CUNG_SERVO_MS : NGHI_DOI_SERVO_MS;
    const uint64_t distance = abs(BUOC_1[k].goc - position[servo]);
    const uint64_t duration = MOVE_SPEED_DEG_S == 0 ? 0 : distance * 1000 / MOVE_SPEED_DEG_S;
    ExpectedMove m = {servo, position[servo], BUOC_1[k].goc,
                      (unsigned long)cursor, (unsigned long)(cursor + duration)};
    moves.push_back(m);
    position[servo] = m.to;
    cursor += duration;
  }
}

int expectedAngle(int servo, unsigned long elapsed) {
  int angle = GOC_BAN_DAU[servo];
  for (size_t k = 0; k < moves.size(); k++) {
    const ExpectedMove &m = moves[k];
    if (elapsed < m.begin) break;
    if (m.servo != servo) continue;
    if (elapsed >= m.end) angle = m.to;  // gồm trường hợp tốc độ 0.
    else {
      const int distance = abs(m.to - m.from);
      const int travelled = (uint64_t)distance * (elapsed - m.begin) / (m.end - m.begin);
      angle = m.from + (m.to > m.from ? travelled : -travelled);
    }
  }
  return angle;
}

bool correctConfiguration() {
  const uint8_t pins[] = {5, 6, 9, 10};
  const int initial[] = {0, 30, 80, 20};
  const int movers[] = {2, 3, 4, 4, 1, 1, 1};
  const int targets[] = {45, 60, 60, 20, 45, 90, 45};
  if (ULONG_MAX != UINT32_MAX || SERVO_COUNT != sizeof(pins) ||
      sizeof(GOC_BAN_DAU) / sizeof(GOC_BAN_DAU[0]) != sizeof(pins) ||
      BUOC_COUNT != sizeof(movers) / sizeof(movers[0]) || SET_HOLD_MS != 1500 ||
      NGHI_DOI_SERVO_MS != 1500 || NGHI_CUNG_SERVO_MS != 500) return false;
  for (int i = 0; i < SERVO_COUNT; i++) {
    if (SERVO_PINS[i] != pins[i] || GOC_BAN_DAU[i] != initial[i]) return false;
  }
  for (int k = 0; k < BUOC_COUNT; k++) {
    if (BUOC_1[k].servo != movers[k] || BUOC_1[k].goc != targets[k]) return false;
  }
#if defined(TEST_17_ZERO_GAP) || defined(TEST_17_ZERO_BOTH)
  const unsigned long gap = 0;
#else
  const unsigned long gap = 500;
#endif
#if defined(TEST_17_ZERO_SPEED) || defined(TEST_17_ZERO_BOTH)
  const unsigned long speed = 0;
#else
  const unsigned long speed = 15;
#endif
  return SET_GAP_MS == gap && MOVE_SPEED_DEG_S == speed;
}

bool runStartup(uint32_t start, unsigned long expectedWrap, const char *name) {
  // Mỗi ca là một lần cấp điện mới. Bốn ca dùng đúng 16 slot của mock.
  const int base = servo_attaches;
  if (base + SERVO_COUNT > 16) {
    printf("KHÔNG ĐẠT: vượt sức chứa mock Servo\n");
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
              servo_trace[base + i].attach_calls == 0 && servo_trace[base + i].output_calls == 0;
  }
  bool planned = buocKetThuc == moves.back().end - moves.front().begin;
  for (size_t k = 0; k < moves.size(); k++) {
    planned = planned && buocTu[k] == moves[k].from &&
              buocBatDau[k] == moves[k].begin - moves.front().begin &&
              buocThoiGian[k] == moves[k].end - moves[k].begin;
  }

  const unsigned long finalEnd = moves.back().end;
  // Chạy một lần rồi dừng: theo dõi hai lần lịch hoàn chỉnh, ít nhất 30 s.
  const unsigned long duration = 2 * finalEnd > 30000UL ? 2 * finalEnd : 30000UL;
  int previous[SERVO_COUNT], previousExpected[SERVO_COUNT];
  int expectedOutputs[SERVO_COUNT] = {};
  bool attachedBefore[SERVO_COUNT] = {};
  for (int i = 0; i < SERVO_COUNT; i++) previous[i] = previousExpected[i] = GOC_BAN_DAU[i];
  int expectedWrites = SERVO_COUNT, maxStep = 0;
  bool schedule = true, angles = true, smooth = true, sequential = true;
  bool onlyChanges = true, stopped = true, finished = true;
  unsigned long firstFailure = duration + 1, actualFinish = duration + 1, wrapAt = 0;
  uint32_t previousMs = start;

  for (unsigned long elapsed = 0; elapsed <= duration; elapsed++) {
    fake_ms = (uint32_t)(start + elapsed);
    if (elapsed != 0 && fake_ms < previousMs) wrapAt = elapsed;
    previousMs = fake_ms;
    const int beforeLoopWrites = servo_writes;
    loop();
    if (xong && actualFinish > duration) actualFinish = elapsed;
    finished = finished && xong == (elapsed >= finalEnd);
    int dueCount = 0, changed = 0, active = 0;
    for (size_t k = 0; k < moves.size(); k++) {
      if (elapsed >= moves[k].begin && elapsed < moves[k].end) active++;
    }
    for (int i = 0; i < SERVO_COUNT; i++) {
      const int slot = base + i;
      const unsigned long deadline = (unsigned long)i * SET_GAP_MS;
      const bool due = elapsed >= deadline;
      const int want = expectedAngle(i, elapsed);
      if (due) {
        dueCount++;
        if (!attachedBefore[i]) {
          attachedBefore[i] = true;
          expectedOutputs[i]++;  // attach phát góc đã write trước đó.
        }
        if (want != previousExpected[i]) {
          expectedWrites++;
          expectedOutputs[i]++;
        }
      }
      previousExpected[i] = want;
      const ServoMockTrace &trace = servo_trace[slot];
      schedule = schedule && servos[i].attached() == due;
      if (!due) {
        schedule = schedule && trace.attach_calls == 0 && trace.output_calls == 0 &&
                   servo_angle[slot] == -123;
      } else {
        schedule = schedule && trace.pin == SERVO_PINS[i] && trace.pin != 11 &&
                   trace.first_attach_ms == (uint32_t)(start + deadline) && trace.attach_calls == 1;
        angles = angles && trace.first_angle == GOC_BAN_DAU[i] && servo_angle[slot] == want &&
                 trace.min_angle >= 0 && trace.max_angle <= 90;
        const int delta = servo_angle[slot] - previous[i];
        if (abs(delta) > maxStep) maxStep = abs(delta);
        if (delta != 0) {
          changed++;
          bool validMove = false;
          for (size_t k = 0; k < moves.size(); k++) {
            const ExpectedMove &m = moves[k];
            if (m.servo != i || elapsed < m.begin || elapsed > m.end) continue;
            const unsigned long distance = abs(m.to - m.from);
            const unsigned long span = m.end - m.begin;
            const int limit = span == 0 ? distance : (distance + span - 1) / span;
            validMove = abs(delta) <= limit &&
                        (m.to > m.from ? delta > 0 : delta < 0) &&
                        servo_angle[slot] >= (m.from < m.to ? m.from : m.to) &&
                        servo_angle[slot] <= (m.from > m.to ? m.from : m.to);
          }
          smooth = smooth && validMove;
        }
        previous[i] = servo_angle[slot];
      }
      // Bao gồm cả write trùng/thoáng qua rồi quay lại góc cũ trong cùng loop.
      onlyChanges = onlyChanges && trace.output_calls == expectedOutputs[i];
    }
    schedule = schedule && servo_attaches == base + dueCount;
    onlyChanges = onlyChanges && servo_writes == writesBefore + expectedWrites;
    sequential = sequential && active <= 1 && changed <= 1;
    if (elapsed > finalEnd) stopped = stopped && servo_writes == beforeLoopWrites;

    // Gọi loop lần nữa cùng ms phải không ghi hoặc attach thêm, kể cả điểm chuyển.
    const int beforeRepeat = servo_writes;
    loop();
    onlyChanges = onlyChanges && servo_writes == beforeRepeat;
    schedule = schedule && servo_attaches == base + dueCount;
    for (int i = 0; i < SERVO_COUNT; i++) {
      onlyChanges = onlyChanges && servo_trace[base + i].output_calls == expectedOutputs[i];
    }
    if (!(startup && planned && schedule && angles && smooth && sequential && onlyChanges && stopped && finished) &&
        firstFailure > duration) firstFailure = elapsed;
  }

  const int finalAngles[] = {45, 45, 60, 20};
  const int idleWrites = servo_writes;
  const uint32_t idleTimes[] = {UINT32_MAX, 0, 1};
  for (size_t j = 0; j < sizeof(idleTimes) / sizeof(idleTimes[0]); j++) {
    fake_ms = idleTimes[j];
    loop();
    stopped = stopped && xong && servo_writes == idleWrites && servo_attaches == base + SERVO_COUNT;
    for (int i = 0; i < SERVO_COUNT; i++) {
      stopped = stopped && servo_angle[base + i] == finalAngles[i] &&
                servo_trace[base + i].output_calls == expectedOutputs[i];
    }
  }
  const bool rollover = wrapAt == expectedWrap;
  printf("%s: start=%lu, GAP=%lu, SPEED=%lu độ/s, theo dõi %lu ms (bước 1 ms)\n",
         name, (unsigned long)start, SET_GAP_MS, MOVE_SPEED_DEG_S, duration);
  printf("  Chưa phát xung trong setup, góc đầu đúng và attach đúng chân/mốc: %s\n",
         startup && schedule ? "đúng" : "SAI");
  printf("  Đủ 7 lượt, góc xuất phát và lịch nghỉ khớp oracle độc lập: %s\n", planned && angles ? "đúng" : "SAI");
  printf("  Đơn điệu theo từng lượt, đúng chiều/giới hạn, bước tối đa %d°: %s\n", maxStep, smooth ? "đúng" : "SAI");
  printf("  Mỗi lúc chỉ một servo quay; chỉ ghi góc đổi: %s\n", sequential && onlyChanges ? "đúng" : "SAI");
  printf("  Xong đúng %lu ms (thực tế %lu), giữ 45/45/60/20°, không ghi tiếp: %s\n",
         finalEnd, actualFinish, finished && stopped ? "đúng" : "SAI");
  if (expectedWrap != 0) printf("  millis() tràn tại %lu ms (cần %lu), lịch tiếp tục đúng: %s\n",
                                wrapAt, expectedWrap, rollover && angles ? "đúng" : "SAI");
  if (firstFailure <= duration) printf("  Lỗi đầu tiên tại elapsed=%lu ms\n", firstFailure);
  return startup && planned && schedule && angles && smooth && sequential && onlyChanges &&
         stopped && finished && rollover;
}

int main() {
  if (!correctConfiguration()) {
    printf("KẾT QUẢ: KHÔNG ĐẠT (sai cấu hình đề bài hoặc millis không phải 32 bit)\n");
    return 1;
  }
  makeExpectedMoves();
  for (size_t k = 0; k < moves.size(); k++) {
    const ExpectedMove &m = moves[k];
    printf("Lượt %u: servo %d %d->%d°, %lu..%lu ms\n",
           (unsigned)k + 1, m.servo + 1, m.from, m.to, m.begin, m.end);
  }
  for (int i = 0; i < 16; i++) servo_angle[i] = -123;
  const bool normal = runStartup(0, 0, "Khởi động thường");
  const unsigned long setWrap = SET_GAP_MS != 0 ? SET_GAP_MS / 2 : SET_HOLD_MS / 2;
  const bool setRollover = runStartup((uint32_t)(0UL - setWrap), setWrap, "Tràn khi đặt góc/chờ");
  const unsigned long upWrap = moves[0].begin + (moves[0].end - moves[0].begin) / 2;
  const bool upRollover = runStartup((uint32_t)(0UL - upWrap), upWrap, "Tràn trong bước 1 (lượt tăng)");
  const unsigned long downWrap = moves[3].begin + (moves[3].end - moves[3].begin) / 2;
  const bool downRollover = runStartup((uint32_t)(0UL - downWrap), downWrap, "Tràn trong bước 1 (lượt giảm)");
  const bool ok = normal && setRollover && upRollover && downRollover;
  printf(ok ? "KẾT QUẢ: ĐẠT\n" : "KẾT QUẢ: KHÔNG ĐẠT\n");
  return ok ? 0 : 1;
}
