// LCD I2C giả lập: chỉ để biên dịch thử
#pragma once
#include <stdint.h>
class LiquidCrystal_I2C {
 public:
  LiquidCrystal_I2C(uint8_t, uint8_t, uint8_t) {}
  void init() {}
  void backlight() {}
  void setCursor(uint8_t, uint8_t) {}
  void print(const char*) {}
};
