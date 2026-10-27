// Arduino giả lập để biên dịch và chạy thử sketch trên máy tính (g++), không dùng cho board thật
#pragma once
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
extern unsigned long fake_ms;                 // thời gian giả, test tự tăng
inline unsigned long millis() { return fake_ms; }
extern int fake_pin[32];                      // giá trị digital/analog đang ghi ở mỗi chân
inline void pinMode(uint8_t, uint8_t) {}
inline void digitalWrite(uint8_t p, int v) { fake_pin[p] = v; }
inline int  digitalRead(uint8_t p) { return fake_pin[p]; }
inline void analogWrite(uint8_t p, int v) { fake_pin[p] = v; }
