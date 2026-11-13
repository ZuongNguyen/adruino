// Kiểm tra từng ms lúc khởi động bình thường và khi millis() tràn giữa chuỗi reset.
// Bản RESET_GAP_MS=0 được run_all.sh sinh trong build/, không sửa sketch gốc.
#ifdef __AVR__
#error "Đây là test trên máy tính, không nạp lên Arduino."
#endif
#include "Arduino.h"
#include "Servo.h"
#include <limits.h>
#ifdef TEST_16_ZERO_GAP
#include "../build/test_16_zero_gap.inc"
#else
#include "../prototype-16-reset-5-servo/sketch.ino"
#endif

bool runStartup(uint32_t start, const char *name) {
  // Mỗi ca dùng đối tượng mới. Các slot đã dùng ở ca trước vẫn giữ nhật ký;
  // hai ca dùng tổng cộng 10/16 slot, không gọi setup() lại trên servo đã attach.
  const int base = servo_attaches;
  const int writesBefore = servo_writes;
  for (int i = 0; i < SERVO_COUNT; i++) servos[i] = Servo();
  fake_ms = start;
  setup();
  bool startup = servo_attaches == base && servo_writes == writesBefore + SERVO_COUNT;
  for (int i = 0; i < SERVO_COUNT; i++) {
    startup = startup && !servos[i].attached() && servo_angle[base + i] == -123 &&
              servo_trace[base + i].attach_calls == 0 &&
              servo_trace[base + i].output_calls == 0;
  }
  const unsigned long lastDeadline = (SERVO_COUNT - 1UL) * RESET_GAP_MS;
  // Ít nhất 3 giây; thêm thời gian giữ sau khi servo cuối tới lượt.
  const unsigned long duration = lastDeadline + 1000UL > 3000UL ?
                                 lastDeadline + 1000UL : 3000UL;
  bool schedule = true, angles = true, once = true;
  unsigned long firstFailure = duration + 1;
  for (unsigned long elapsed = 0; elapsed <= duration; elapsed++) {
    fake_ms = (uint32_t)(start + elapsed);
    loop();
    // Gọi lại cùng ms để bắt trường hợp attach hoặc write lặp trong loop().
    loop();
    int expectedCount = 0;
    for (int i = 0; i < SERVO_COUNT; i++) {
      const int slot = base + i;
      const unsigned long deadline = (unsigned long)i * RESET_GAP_MS;
      const bool due = elapsed >= deadline;
      if (due) expectedCount++;
      schedule = schedule && servos[i].attached() == due;
      const ServoMockTrace &trace = servo_trace[slot];
      if (due) {
        schedule = schedule && trace.pin == SERVO_PINS[i] &&
                   trace.first_attach_ms == (uint32_t)(start + deadline);
        angles = angles && trace.first_angle == 0 && servo_angle[slot] == 0 &&
                 trace.min_angle == 0 && trace.max_angle == 0;
        once = once && trace.attach_calls == 1 && trace.output_calls == 1;
      } else {
        schedule = schedule && trace.attach_calls == 0 && trace.output_calls == 0 &&
                   servo_angle[slot] == -123;
      }
    }
    once = once && servo_attaches == base + expectedCount &&
           servo_writes == writesBefore + SERVO_COUNT;
    if (!(startup && schedule && angles && once) && firstFailure > duration) {
      firstFailure = elapsed;
    }
  }
  // Sau khi hoàn tất, đi qua mốc tràn lần nữa cũng không được reset lại.
  const uint32_t idleTimes[] = {UINT32_MAX, 0, 1};
  for (unsigned int j = 0; j < sizeof(idleTimes) / sizeof(idleTimes[0]); j++) {
    fake_ms = idleTimes[j];
    loop();
    once = once && servo_attaches == base + SERVO_COUNT &&
           servo_writes == writesBefore + SERVO_COUNT;
    for (int i = 0; i < SERVO_COUNT; i++) {
      angles = angles && servo_angle[base + i] == 0 &&
               servo_trace[base + i].min_angle == 0 && servo_trace[base + i].max_angle == 0;
      once = once && servos[i].attached() && servo_trace[base + i].attach_calls == 1 &&
             servo_trace[base + i].output_calls == 1;
    }
  }
  printf("%s: start=%lu, GAP=%lu, theo dõi %lu ms (bước 1 ms)\n",
         name, (unsigned long)start, RESET_GAP_MS, duration);
  printf("  setup chưa phát đầu ra: %s; đúng chân/thứ tự/thời điểm: %s\n",
         startup ? "đúng" : "SAI", schedule ? "đúng" : "SAI");
  printf("  Góc đầu và mọi đầu ra đều 0°: %s; không attach/write lại: %s\n",
         angles ? "đúng" : "SAI", once ? "đúng" : "SAI");
  if (firstFailure <= duration) printf("  Sai đầu tiên tại elapsed=%lu ms\n", firstFailure);
  return startup && schedule && angles && once;
}

int main() {
  const uint8_t expectedPins[] = {5, 6, 9, 10, 11};
  if (ULONG_MAX != UINT32_MAX || SERVO_COUNT != sizeof(expectedPins) || RESET_ANGLE != 0) {
    printf("KẾT QUẢ: KHÔNG ĐẠT (cần millis unsigned long 32 bit, 5 servo và góc 0°)\n");
    return 1;
  }
  for (int i = 0; i < SERVO_COUNT; i++) {
    if (SERVO_PINS[i] != expectedPins[i]) {
      printf("KẾT QUẢ: KHÔNG ĐẠT (sai chân servo %d)\n", i + 1);
      return 1;
    }
  }
#ifdef TEST_16_ZERO_GAP
  if (RESET_GAP_MS != 0) {
    printf("KẾT QUẢ: KHÔNG ĐẠT (biến thể gap 0 chưa đổi hằng)\n");
    return 1;
  }
#else
  // Khóa cấu hình đề bài; các mốc kiểm tra bên trên vẫn tính từ hằng sketch.
  if (RESET_GAP_MS != 500) {
    printf("KẾT QUẢ: KHÔNG ĐẠT (cấu hình mặc định phải cách nhau 500 ms)\n");
    return 1;
  }
#endif
  for (int i = 0; i < 16; i++) servo_angle[i] = -123;
  const bool normal = runStartup(0, "Khởi động thường");
  // Nếu attach lặp làm tăng sai base thì không dùng nó để truy cập ca kế tiếp.
  if (!normal) {
    printf("KẾT QUẢ: KHÔNG ĐẠT\n");
    return 1;
  }
  // Tràn giữa mốc servo 1 và servo 2 khi GAP > 0, vẫn theo dõi đủ 3 giây nếu GAP = 0.
  const uint32_t nearWrap = UINT32_MAX - (uint32_t)(RESET_GAP_MS / 2);
  const bool rollover = runStartup(nearWrap, "Khởi động gần tràn millis");
  const bool ok = normal && rollover;
  printf(ok ? "KẾT QUẢ: ĐẠT\n" : "KẾT QUẢ: KHÔNG ĐẠT\n");
  return ok ? 0 : 1;
}
