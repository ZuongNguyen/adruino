// Servo giả lập: ghi lại góc gần nhất của từng servo theo thứ tự attach()
#pragma once
#include <stdint.h>
extern int servo_angle[16];
extern int servo_writes;
class Servo {
  int idx = -1;
  static int count;
 public:
  void attach(int) { idx = count++; }
  void attach(int, int, int) { idx = count++; }
  void write(int a) { servo_angle[idx] = a; servo_writes++; }
};
