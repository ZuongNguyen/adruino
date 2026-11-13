// Servo giả lập: chỉ ghi đầu ra sau attach(), giữ góc chờ nếu write() trước đó.
// Chỉ số quan sát vẫn theo thứ tự attach() để tương thích các test cũ.
// Đây là nhật ký lời gọi, không mô phỏng ngắt Timer1 hoặc xung điện thực.
#pragma once
#include <stdint.h>
#include <assert.h>
#include "Arduino.h"
struct ServoMockTrace {
  int pin;
  unsigned long first_attach_ms;
  int first_angle;
  int attach_calls;
  int output_calls;
  int min_angle;
  int max_angle;
};
extern int servo_angle[16];
extern int servo_writes;
extern int servo_attaches;
extern ServoMockTrace servo_trace[16];
class Servo {
  int idx = -1;
  int pendingAngle = 90;
  static int count;

  void applyAngle() {
    ServoMockTrace &trace = servo_trace[idx];
    if (trace.output_calls == 0) {
      trace.min_angle = trace.max_angle = pendingAngle;
    } else {
      if (pendingAngle < trace.min_angle) trace.min_angle = pendingAngle;
      if (pendingAngle > trace.max_angle) trace.max_angle = pendingAngle;
    }
    servo_angle[idx] = pendingAngle;
    trace.output_calls++;
  }
 public:
  void attach(int pin) {
    if (idx < 0) {
      assert(count < 16);
      idx = count++;
      servo_trace[idx].first_attach_ms = millis();
      servo_trace[idx].first_angle = pendingAngle;
    }
    servo_trace[idx].pin = pin;
    servo_trace[idx].attach_calls++;
    servo_attaches++;
    applyAngle();
  }
  void attach(int pin, int, int) { attach(pin); }
  bool attached() const { return idx >= 0; }
  void write(int a) {
    pendingAngle = a;
    servo_writes++;
    if (attached()) applyAngle();
  }
};
