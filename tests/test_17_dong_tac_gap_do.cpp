// Test bản 17 từng ms trong hai lần thời gian hoàn chỉnh: nhóm, tốc độ, về base và tràn millis().
// Oracle lập từng nhóm từ bảng bằng trạng thái riêng và số học 64 bit, không gọi hàm lập lịch/nội suy của sketch.
#ifdef __AVR__
#error "Test chạy trên máy tính, không nạp lên Arduino."
#endif
#include "Arduino.h"
#include "Servo.h"
#include <limits.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#if defined(TEST_17_ZERO_BOTH)
#include "../build/test_17_zero_both.inc"
#elif defined(TEST_17_ZERO_GAP)
#include "../build/test_17_zero_gap.inc"
#elif defined(TEST_17_ZERO_SPEED)
#include "../build/test_17_zero_speed.inc"
#elif defined(TEST_17_CUSTOM_TABLE)
#include "../build/test_17_custom_table.inc"
#elif defined(TEST_17_EMPTY_TABLE)
#include "../build/test_17_empty_table.inc"
#elif defined(TEST_17_INVALID_TABLE)
#include "../build/test_17_invalid_table.inc"
#else
#include "../prototype-17-dong-tac-gap-do/sketch.ino"
#endif

struct ExpectedMove {
  int servo, from, to, group, speedRow;
  unsigned long speed;
  uint64_t begin, end;
  bool returning;
};
std::vector<ExpectedMove> moves;
const size_t SPEED_ROWS = sizeof(TOC_DO_DEG_S) / sizeof(TOC_DO_DEG_S[0]);
const int UNATTACHED_ANGLE = -123;
uint64_t moveStart, finalEnd;
bool duplicateFallback = false, longestNotLast = false;
int invalidRows = 0, invalidStepRows = 0, invalidServoRows = 0, userRows = 0, returnRows = 0;

uint32_t clockAt(uint32_t start, uint64_t elapsed) {
  return (uint32_t)((uint64_t)start + elapsed);
}
uint64_t attachAt(int servo) { return (uint64_t)servo * SET_GAP_MS; }

ExpectedMove expectedMove(int servo, int from, int to, int group, int speedRow, uint64_t begin, bool returning) {
  const unsigned long speed = TOC_DO_DEG_S[speedRow][servo];
  const uint64_t distance = abs(to - from);
  const uint64_t span = speed == 0 ? 0 : distance * 1000 / speed;
  ExpectedMove m = {servo, from, to, group, speedRow, speed, begin, begin + span, returning};
  return m;
}

void makeExpectedMoves() {
  moves.clear();
  duplicateFallback = longestNotLast = false;
  invalidRows = invalidStepRows = invalidServoRows = userRows = returnRows = 0;
  std::vector<BuocQuay> valid;
  for (size_t k = 0; k < sizeof(DONG_TAC_1) / sizeof(DONG_TAC_1[0]); ++k) {
    const bool badServo = DONG_TAC_1[k].servo < 1 || DONG_TAC_1[k].servo > SERVO_COUNT;
    const bool badStep = DONG_TAC_1[k].buoc < 1 || DONG_TAC_1[k].buoc >= SPEED_ROWS;
    if (badServo) ++invalidServoRows;
    if (badStep) ++invalidStepRows;
    if (badServo || badStep) ++invalidRows;
    else valid.push_back(DONG_TAC_1[k]);
  }
  userRows = (int)valid.size();
  int position[SERVO_COUNT];
  for (int i = 0; i < SERVO_COUNT; ++i) position[i] = GOC_BASE[i];
  moveStart = attachAt(SERVO_COUNT - 1) + SET_HOLD_MS;
  finalEnd = moveStart;
  int group = 0;
  // Chia các dòng hợp lệ thành nhóm trước, rồi lấy thời điểm kết thúc lớn nhất trong từng nhóm.
  for (size_t first = 0; first < valid.size();) {
    size_t past = first + 1;
    std::vector<int> members(1, valid[first].servo);
    while (past < valid.size() && valid[past].cungLuc) {
      if (std::find(members.begin(), members.end(), valid[past].servo) != members.end()) {
        duplicateFallback = true;
        break;
      }
      members.push_back(valid[past++].servo);
    }
    const uint64_t begin = first == 0 ? moveStart : finalEnd +
      (valid[first].servo == valid[first - 1].servo ? NGHI_CUNG_SERVO_MS : NGHI_DOI_SERVO_MS);
    uint64_t end = begin;
    for (size_t row = first; row < past; ++row) {
      const int i = valid[row].servo - 1;
      ExpectedMove m = expectedMove(i, position[i], valid[row].goc, group, valid[row].buoc - 1, begin, false);
      moves.push_back(m);
      position[i] = m.to;
      end = std::max(end, m.end);
    }
    if (past - first > 1 && moves.back().end < end) longestNotLast = true;
    finalEnd = end;
    first = past;
    ++group;
  }
  const uint64_t returnBegin = finalEnd + NGHI_DOI_SERVO_MS;
  for (int i = 0; i < SERVO_COUNT; ++i) {
    if (position[i] == GOC_BASE[i]) continue;
    ExpectedMove m = expectedMove(i, position[i], GOC_BASE[i], group, SPEED_ROWS - 1, returnBegin, true);
    moves.push_back(m);
    finalEnd = std::max(finalEnd, m.end);
    ++returnRows;
  }
}

int expectedAngle(int servo, uint64_t elapsed) {
  int angle = GOC_BASE[servo];
  for (size_t k = 0; k < moves.size(); ++k) {
    const ExpectedMove &m = moves[k];
    if (elapsed < m.begin) break;
    if (m.servo != servo) continue;
    if (elapsed >= m.end) angle = m.to; // Bao gồm tốc độ 0: đến đích ngay.
    else {
      const uint64_t travelled = (uint64_t)abs(m.to - m.from) * (elapsed - m.begin) / (m.end - m.begin);
      angle = m.from + (m.to >= m.from ? (int)travelled : -(int)travelled);
    }
  }
  return angle;
}

bool correctConfiguration() {
  const uint8_t pins[] = {5, 6, 9, 10};
  const int base[] = {90, 15, 30, 0};
  const unsigned long speeds[][4] = {
#if defined(TEST_17_CUSTOM_TABLE)
    // Mọi ô khác nhau: bắt lấy nhầm hàng bước 2/3/về base, vốn trùng tốc độ ở cấu hình mặc định.
    {5, 10, 20, 25}, {40, 50, 100, 125}, {200, 250, 500, 1000}, {1, 2, 4, 8}
#else
    {50, 50, 50, 50}, {30, 20, 20, 30}, {30, 20, 20, 30}, {30, 20, 20, 30}
#endif
  };
  if (ULONG_MAX != UINT32_MAX || SERVO_COUNT != sizeof(pins) ||
      sizeof(GOC_BASE) / sizeof(GOC_BASE[0]) != SERVO_COUNT ||
      SPEED_ROWS != sizeof(speeds) / sizeof(speeds[0]) || HANG_VE_BASE != SPEED_ROWS - 1 ||
      sizeof(TOC_DO_DEG_S[0]) / sizeof(TOC_DO_DEG_S[0][0]) != SERVO_COUNT ||
      BUOC_COUNT != sizeof(DONG_TAC_1) / sizeof(DONG_TAC_1[0]) ||
      LICH_MAX != BUOC_COUNT + SERVO_COUNT ||
      sizeof(lichServo) / sizeof(lichServo[0]) != LICH_MAX ||
      sizeof(lichTu) / sizeof(lichTu[0]) != LICH_MAX ||
      sizeof(lichDen) / sizeof(lichDen[0]) != LICH_MAX ||
      sizeof(lichBatDau) / sizeof(lichBatDau[0]) != LICH_MAX ||
      sizeof(lichThoiGian) / sizeof(lichThoiGian[0]) != LICH_MAX || SET_HOLD_MS != 1500 ||
      NGHI_DOI_SERVO_MS != 1500 || NGHI_CUNG_SERVO_MS != 500 || LAN_LUOT || !CUNG_LUC) return false;
  for (int i = 0; i < SERVO_COUNT; ++i) {
    if (SERVO_PINS[i] != pins[i] || GOC_BASE[i] != base[i]) return false;
    for (size_t row = 0; row < SPEED_ROWS; ++row) {
#if defined(TEST_17_ZERO_SPEED) || defined(TEST_17_ZERO_BOTH)
      if (TOC_DO_DEG_S[row][i] != 0) return false;
#else
      if (TOC_DO_DEG_S[row][i] != speeds[row][i]) return false;
#endif
    }
  }
#if defined(TEST_17_ZERO_GAP) || defined(TEST_17_ZERO_BOTH)
  if (SET_GAP_MS != 0) return false;
#else
  if (SET_GAP_MS != 500) return false;
#endif
#if !defined(TEST_17_CUSTOM_TABLE) && !defined(TEST_17_EMPTY_TABLE) && !defined(TEST_17_INVALID_TABLE)
  // Dữ liệu đề bài được đối chiếu riêng; mọi mốc/thời lượng kiểm thử vẫn tính từ sketch.
  const int steps[] = {1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3};
  const int movers[] = {3, 2, 1, 1, 1, 1, 4, 4, 2, 3, 4, 2, 3, 1, 2, 3, 4, 2, 3};
  const int targets[] = {60, 45, 135, 90, 45, 90, 180, 0, 130, 120, 120, 45, 60, 180, 130, 120, 0, 45, 60};
  const int simultaneousRows[] = {9, 12, 15, 18}; // chỉ số từ 0 của các dòng servo 3 đi cùng servo 2
  if (BUOC_COUNT != sizeof(movers) / sizeof(movers[0])) return false;
  for (int k = 0; k < BUOC_COUNT; ++k) {
    const bool simultaneous = std::find(simultaneousRows, simultaneousRows +
      sizeof(simultaneousRows) / sizeof(simultaneousRows[0]), k) != simultaneousRows +
      sizeof(simultaneousRows) / sizeof(simultaneousRows[0]);
    if (DONG_TAC_1[k].buoc != steps[k] || DONG_TAC_1[k].servo != movers[k] || DONG_TAC_1[k].goc != targets[k] ||
        DONG_TAC_1[k].cungLuc != simultaneous) return false;
  }
#endif
  return true;
}

bool fixtureCoverage() {
#if defined(TEST_17_CUSTOM_TABLE)
  bool seenSteps[SPEED_ROWS] = {};
  bool mixedStepsInGroup = false;
  for (size_t k = 0; k < moves.size(); ++k) {
    seenSteps[moves[k].speedRow] = true;
    if (k && moves[k].group == moves[k - 1].group && moves[k].speedRow != moves[k - 1].speedRow)
      mixedStepsInGroup = true;
  }
  for (size_t row = 0; row < SPEED_ROWS; ++row) if (!seenSteps[row]) return false;
  return duplicateFallback && longestNotLast && mixedStepsInGroup &&
    returnRows == SERVO_COUNT && invalidRows == 0;
#elif defined(TEST_17_EMPTY_TABLE)
  return BUOC_COUNT == 0 && moves.empty() && returnRows == 0;
#elif defined(TEST_17_INVALID_TABLE)
  bool zero = false, tooLarge = false, largest = false;
  bool zeroStep = false, tooLargeStep = false, largestStep = false;
  for (int k = 0; k < BUOC_COUNT; ++k) {
    zero = zero || DONG_TAC_1[k].servo == 0;
    tooLarge = tooLarge || DONG_TAC_1[k].servo == SERVO_COUNT + 1;
    largest = largest || DONG_TAC_1[k].servo == UINT8_MAX;
    // Bước sai phải đi kèm số servo đúng để test thực sự đi qua kiểm tra số bước.
    if (DONG_TAC_1[k].servo >= 1 && DONG_TAC_1[k].servo <= SERVO_COUNT) {
      zeroStep = zeroStep || DONG_TAC_1[k].buoc == 0;
      tooLargeStep = tooLargeStep || DONG_TAC_1[k].buoc == SPEED_ROWS;
      largestStep = largestStep || DONG_TAC_1[k].buoc == UINT8_MAX;
    }
  }
  return zero && tooLarge && largest && zeroStep && tooLargeStep && largestStep &&
    invalidServoRows > 0 && invalidStepRows > 0 && userRows > 0 && returnRows > 0;
#else
  return invalidRows == 0 && returnRows == 3 && moves.size() == BUOC_COUNT + 3U &&
    moves[userRows].returning && moves[userRows].servo == 0;
#endif
}

bool checkPlan() {
  if (lichCount > LICH_MAX || moves.size() > LICH_MAX ||
      lichCount != moves.size() || lichKetThuc != finalEnd - moveStart) return false;
  for (size_t k = 0; k < moves.size(); ++k) {
    const ExpectedMove &m = moves[k];
    if (lichServo[k] != m.servo || lichTu[k] != m.from || lichDen[k] != m.to ||
        lichBatDau[k] != m.begin - moveStart || lichThoiGian[k] != m.end - m.begin) {
      printf("  Lịch sai ở dòng %u: servo=%u, từ=%d, đến=%d, bắt đầu=%lu, thời lượng=%lu; cần hàng %d tốc độ %lu\n",
        (unsigned)k + 1, lichServo[k] + 1, lichTu[k], lichDen[k], lichBatDau[k], lichThoiGian[k],
        m.speedRow + 1, m.speed);
      return false;
    }
  }
  return true;
}

bool holdsAngle(int slot, int angle, uint64_t elapsed, uint64_t begin, uint64_t end) {
  // Loại đầu mốc quay kế tiếp để dùng cả biến thể tốc độ 0 (đến đích ngay tại mốc đó).
  return elapsed < begin || elapsed >= end || servo_angle[slot] == angle;
}

bool checkStep3Holds(int baseSlot, uint64_t elapsed) {
#if !defined(TEST_17_CUSTOM_TABLE) && !defined(TEST_17_EMPTY_TABLE) && !defined(TEST_17_INVALID_TABLE)
  // Tính mốc từ lịch độc lập, không gõ cứng thời gian và không đọc lich*[] của sketch.
  // Tách servo 2/3 vì servo 3 kết thúc bước 2 sớm hơn servo 2.
  return holdsAngle(baseSlot + 1, 130, elapsed, moves[8].end, moves[11].begin) &&
    holdsAngle(baseSlot + 2, 120, elapsed, moves[9].end, moves[12].begin) &&
    holdsAngle(baseSlot, 180, elapsed, moves[13].end, moves[userRows].begin) &&
    holdsAngle(baseSlot + 3, 120, elapsed, moves[10].end, moves[16].begin);
#else
  return true;
#endif
}

bool runStartup(uint32_t start, unsigned long expectedWrap, const char *name) {
  // Bốn ca dùng đúng 16 slot mock. Khôi phục globals như một lần cấp điện mới.
  const int baseSlot = servo_attaches, writesBefore = servo_writes;
  if (baseSlot + SERVO_COUNT > 16) return false;
  for (int i = 0; i < SERVO_COUNT; ++i) servos[i] = Servo();
  lichCount = 0;
  lichKetThuc = 0;
  xong = false;
  fake_ms = start;
  setup();
  bool startup = servo_attaches == baseSlot && servo_writes == writesBefore + SERVO_COUNT &&
    !xong && mocKhoiDong == start;
  for (int i = 0; i < SERVO_COUNT; ++i) {
    startup = startup && !servos[i].attached() && !daDat[i] && lastAngle[i] == GOC_BASE[i] &&
      servo_angle[baseSlot + i] == UNATTACHED_ANGLE && servo_trace[baseSlot + i].output_calls == 0;
  }
  const bool planned = checkPlan();
  const unsigned long duration = (unsigned long)(2 * std::max(finalEnd, uint64_t(1)));
  int previous[SERVO_COUNT], previousExpected[SERVO_COUNT], outputs[SERVO_COUNT] = {};
  for (int i = 0; i < SERVO_COUNT; ++i) previous[i] = previousExpected[i] = GOC_BASE[i];
  int expectedWrites = SERVO_COUNT, maxStep = 0;
  bool schedule = true, angles = true, smooth = true, groups = true, onlyChanges = true;
  bool stopped = true, finished = true, finalBase = true, holds = true;
  unsigned long firstFailure = duration + 1, actualFinish = duration + 1, wrapAt = 0;
  uint32_t previousMs = start;
  for (unsigned long elapsed = 0; elapsed <= duration; ++elapsed) {
    fake_ms = clockAt(start, elapsed);
    if (elapsed && fake_ms < previousMs) wrapAt = elapsed;
    previousMs = fake_ms;
    const int writesBeforeLoop = servo_writes;
    loop();
    if (xong && actualFinish > duration) actualFinish = elapsed;
    finished = finished && xong == (elapsed >= finalEnd);
    int dueCount = 0, changedGroup = -1, activeGroup = -1;
    for (size_t k = 0; k < moves.size(); ++k) {
      const ExpectedMove &m = moves[k];
      if (m.from == m.to || elapsed < m.begin || elapsed >= m.end) continue;
      if (activeGroup < 0) activeGroup = m.group;
      groups = groups && activeGroup == m.group;
    }
    for (int i = 0; i < SERVO_COUNT; ++i) {
      const int slot = baseSlot + i;
      const bool due = elapsed >= attachAt(i);
      const int want = expectedAngle(i, elapsed);
      const ServoMockTrace &trace = servo_trace[slot];
      schedule = schedule && servos[i].attached() == due && daDat[i] == due;
      if (!due) {
        schedule = schedule && trace.attach_calls == 0 && trace.output_calls == 0 &&
          servo_angle[slot] == UNATTACHED_ANGLE;
      } else {
        ++dueCount;
        if (elapsed == attachAt(i)) ++outputs[i]; // attach xuất góc đã ghi trong setup.
        if (want != previousExpected[i]) { ++expectedWrites; ++outputs[i]; }
        schedule = schedule && trace.pin == SERVO_PINS[i] && trace.pin != 11 && trace.attach_calls == 1 &&
          trace.first_attach_ms == clockAt(start, attachAt(i));
        angles = angles && trace.first_angle == GOC_BASE[i] && servo_angle[slot] == want &&
          lastAngle[i] == want && trace.min_angle >= 0 && trace.max_angle <= 180;
        const int delta = servo_angle[slot] - previous[i];
        maxStep = std::max(maxStep, abs(delta));
        if (delta) {
          bool validMove = false;
          for (size_t k = 0; k < moves.size(); ++k) {
            const ExpectedMove &m = moves[k];
            if (m.servo != i || elapsed < m.begin || elapsed > m.end || m.from == m.to) continue;
            const uint64_t speed = m.speed;
            const uint64_t limit = speed == 0 ? abs(m.to - m.from) : (speed + 999) / 1000;
            // Góc lệnh là số nguyên: cho phép lượng tử hóa tối đa dưới 1 độ khi xét tốc độ tích lũy.
            const uint64_t travelled = abs(servo_angle[slot] - m.from);
            validMove = (uint64_t)abs(delta) <= limit && (m.to > m.from ? delta > 0 : delta < 0) &&
              servo_angle[slot] >= std::min(m.from, m.to) && servo_angle[slot] <= std::max(m.from, m.to) &&
              (speed == 0 || travelled <= (speed * (elapsed - m.begin) + 999) / 1000);
            if (changedGroup < 0) changedGroup = m.group;
            groups = groups && changedGroup == m.group;
            break;
          }
          smooth = smooth && validMove;
        }
        previous[i] = servo_angle[slot];
      }
      previousExpected[i] = want;
      onlyChanges = onlyChanges && trace.output_calls == outputs[i];
      if (elapsed >= finalEnd) finalBase = finalBase && servo_angle[slot] == GOC_BASE[i];
    }
    schedule = schedule && servo_attaches == baseSlot + dueCount;
    onlyChanges = onlyChanges && servo_writes == writesBefore + expectedWrites;
    if (elapsed > finalEnd) stopped = stopped && servo_writes == writesBeforeLoop;
    // Lặp loop trong cùng ms để bắt ghi trùng, ghi thoáng qua, attach lại ở cả điểm chuyển.
    const int beforeRepeat = servo_writes;
    loop();
    onlyChanges = onlyChanges && servo_writes == beforeRepeat;
    schedule = schedule && servo_attaches == baseSlot + dueCount;
    finished = finished && xong == (elapsed >= finalEnd);
    for (int i = 0; i < SERVO_COUNT; ++i) {
      onlyChanges = onlyChanges && servo_trace[baseSlot + i].output_calls == outputs[i];
      angles = angles && servo_angle[baseSlot + i] ==
        (elapsed >= attachAt(i) ? expectedAngle(i, elapsed) : UNATTACHED_ANGLE);
    }
    for (int slot = 0; slot < servo_attaches && slot < 16; ++slot)
      schedule = schedule && servo_trace[slot].pin != 11;
    holds = checkStep3Holds(baseSlot, elapsed) && holds;
    if (!(startup && planned && schedule && angles && smooth && groups && onlyChanges && stopped && finished && finalBase && holds) &&
        firstFailure > duration) firstFailure = elapsed;
  }
  const int idleWrites = servo_writes;
  const uint32_t idleTimes[] = {UINT32_MAX - 1, UINT32_MAX, 0, 1};
  for (size_t j = 0; j < sizeof(idleTimes) / sizeof(idleTimes[0]); ++j) {
    fake_ms = idleTimes[j];
    loop();
    stopped = stopped && xong && servo_writes == idleWrites && servo_attaches == baseSlot + SERVO_COUNT;
    for (int i = 0; i < SERVO_COUNT; ++i) {
      stopped = stopped && servo_angle[baseSlot + i] == GOC_BASE[i] &&
        servo_trace[baseSlot + i].output_calls == outputs[i] && servo_trace[baseSlot + i].attach_calls == 1;
    }
  }
  const bool rollover = wrapAt == expectedWrap;
  printf("%s: start=%lu, theo dõi %lu ms (bước 1 ms)\n", name, (unsigned long)start, duration);
  printf("  setup/attach/base/chân 11: %s; lịch nhóm/nghỉ độc lập: %s\n",
    startup && schedule ? "đúng" : "SAI", planned ? "đúng" : "SAI");
  printf("  Góc 0–180, đơn điệu, tốc độ đúng bước/servo, bước tối đa %d°: %s; đồng thời đúng nhóm: %s\n",
    maxStep, angles && smooth ? "đúng" : "SAI", groups ? "đúng" : "SAI");
  printf("  Chỉ ghi góc đổi: %s; xong tại %lu ms (thực tế %lu), về mọi base, không ghi tiếp: %s\n",
    onlyChanges ? "đúng" : "SAI", (unsigned long)finalEnd, actualFinish,
    finished && stopped && finalBase ? "đúng" : "SAI");
#if !defined(TEST_17_CUSTOM_TABLE) && !defined(TEST_17_EMPTY_TABLE) && !defined(TEST_17_INVALID_TABLE)
  printf("  Servo 2/3 giữ góc bước 2 sang bước 3; servo 1 giữ 180°, servo 4 giữ 120°: %s\n",
    holds ? "đúng" : "SAI");
#endif
  if (expectedWrap) printf("  Tràn millis tại %lu ms (cần %lu): %s\n", wrapAt, expectedWrap, rollover ? "đúng" : "SAI");
  if (firstFailure <= duration) printf("  Lỗi đầu tiên tại elapsed=%lu ms\n", firstFailure);
  return startup && planned && schedule && angles && smooth && groups && onlyChanges && stopped && finished && finalBase && holds && rollover;
}

unsigned long groupWrap(bool returning) {
  for (size_t k = 0; k < moves.size(); ++k) {
    const ExpectedMove &m = moves[k];
    if (m.returning != returning) continue;
    bool simultaneous = false;
    for (size_t j = 0; j < moves.size(); ++j)
      simultaneous = simultaneous || (j != k && moves[j].group == m.group);
    if (returning || simultaneous) return (unsigned long)(m.begin + (m.end - m.begin) / 2);
  }
  return (unsigned long)std::max(uint64_t(1), finalEnd / (returning ? 2 : 3));
}

int main() {
  if (!correctConfiguration()) {
    printf("KẾT QUẢ: KHÔNG ĐẠT (sai cấu hình đề bài hoặc millis không phải 32 bit)\n");
    return 1;
  }
  makeExpectedMoves();
  if (!fixtureCoverage() || finalEnd >= UINT32_MAX / 2) {
    printf("KẾT QUẢ: KHÔNG ĐẠT (biến thể thiếu tình huống cần kiểm hoặc thời gian vượt phạm vi test)\n");
    return 1;
  }
  printf("Bảng %u dòng hợp lệ, %d dòng sai bỏ qua (bước sai %d, servo sai %d); tự thêm %d dòng về base; GAP=%lu\n",
    (unsigned)userRows, invalidRows, invalidStepRows, invalidServoRows, returnRows, SET_GAP_MS);
  for (int i = 0; i < SERVO_COUNT; ++i) printf("Servo %d: chân %d, base %d°, attach %lu ms\n",
    i + 1, SERVO_PINS[i], GOC_BASE[i], (unsigned long)attachAt(i));
  for (size_t k = 0; k < moves.size(); ++k) {
    const ExpectedMove &m = moves[k];
    printf("Lượt %u, nhóm %d%s: servo %d %d->%d°, hàng tốc độ %d = %lu°/s, %lu..%lu ms\n", (unsigned)k + 1,
      m.group + 1, m.returning ? " về base" : "", m.servo + 1, m.from, m.to,
      m.speedRow + 1, m.speed, (unsigned long)m.begin, (unsigned long)m.end);
  }
  for (int i = 0; i < 16; ++i) servo_angle[i] = UNATTACHED_ANGLE;
  const bool normal = runStartup(0, 0, "Khởi động thường");
  const unsigned long setWrap = (unsigned long)(SET_GAP_MS ? SET_GAP_MS : std::max(1UL, SET_HOLD_MS / 2));
  const bool set = runStartup(clockAt(0, uint64_t(UINT32_MAX) + 1 - setWrap), setWrap, "Tràn tại attach/chờ");
  const unsigned long togetherWrap = groupWrap(false), returnWrap = groupWrap(true);
  const bool together = runStartup(clockAt(0, uint64_t(UINT32_MAX) + 1 - togetherWrap), togetherWrap, "Tràn trong nhóm cùng lúc");
  const bool back = runStartup(clockAt(0, uint64_t(UINT32_MAX) + 1 - returnWrap), returnWrap, "Tràn trong phần về base");
  const bool ok = normal && set && together && back;
  printf(ok ? "KẾT QUẢ: ĐẠT\n" : "KẾT QUẢ: KHÔNG ĐẠT\n");
  return ok ? 0 : 1;
}
