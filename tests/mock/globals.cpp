// Biến dùng chung cho các mock
unsigned long fake_ms = 0;
int fake_pin[32];
int servo_angle[16];
int servo_writes = 0;
#include "Servo.h"
int Servo::count = 0;
