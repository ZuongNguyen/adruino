// Hồi quy mock: write trước attach không được ghi ra ngoài mảng hoặc phát đầu ra.
#include "Arduino.h"
#include "Servo.h"

int main() {
  for (int i = 0; i < 16; i++) servo_angle[i] = -123;
  Servo pending, defaultAngle, calibrated;
  fake_ms = 17;
  pending.write(45);
  pending.write(0);
  calibrated.write(27);
  bool ok = !pending.attached() && !calibrated.attached() &&
            servo_writes == 3 && servo_attaches == 0;
  for (int i = 0; i < 16; i++) {
    ok = ok && servo_angle[i] == -123 && servo_trace[i].output_calls == 0;
  }
  printf("write trước attach chỉ lưu góc chờ: %s\n", ok ? "đúng" : "SAI");

  fake_ms = 29;
  pending.attach(5);
  bool applied = pending.attached() && servo_angle[0] == 0 &&
                 servo_trace[0].first_angle == 0 && servo_trace[0].pin == 5 &&
                 servo_trace[0].first_attach_ms == fake_ms && servo_writes == 3;
  defaultAngle.attach(6);
  calibrated.attach(9, 544, 2400);
  bool defaults = servo_angle[1] == 90 && servo_trace[1].first_angle == 90 &&
                  servo_angle[2] == 27 && servo_trace[2].pin == 9 &&
                  servo_trace[2].first_attach_ms == fake_ms;
  fake_ms = 31;
  pending.write(90);
  pending.write(0); // Cùng một ms: nhật ký vẫn phải thấy góc 90° thoáng qua.
  pending.attach(10);
  bool history = servo_angle[0] == 0 && servo_trace[0].min_angle == 0 &&
                 servo_trace[0].max_angle == 90 && servo_trace[0].first_angle == 0 &&
                 servo_trace[0].first_attach_ms == 29 && servo_trace[0].pin == 10 &&
                 servo_trace[0].attach_calls == 2 && servo_trace[0].output_calls == 4 &&
                 servo_attaches == 4 && servo_writes == 5 && servo_angle[3] == -123;
  printf("attach áp góc chờ, mặc định 90°, overload 3 tham số: %s\n",
         applied && defaults ? "đúng" : "SAI");
  printf("Ghi nhận góc thoáng qua và tái attach cùng đối tượng: %s\n",
         history ? "đúng" : "SAI");
  ok = ok && applied && defaults && history;
  printf(ok ? "KẾT QUẢ: ĐẠT\n" : "KẾT QUẢ: KHÔNG ĐẠT\n");
  return ok ? 0 : 1;
}
