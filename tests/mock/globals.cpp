// Biến dùng chung cho các mock
unsigned long fake_ms = 0;
int fake_pin[32];
int servo_angle[16];
int servo_writes = 0;
#include "Servo.h"
int Servo::count = 0;
#include "Adafruit_PWMServoDriver.h"
bool pca_present = true;
float pca_freq = 0;
uint32_t pca_osc = 0;
int pca_ticks[16];
int pca_on[16];
int pca_writes[16];
