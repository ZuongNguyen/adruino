// PCA9685 giả lập để chạy thử sketch trên máy tính (g++), không dùng cho board thật
#pragma once
#include <stdint.h>
extern bool pca_present;                      // false: giả lập không thấy chip trên I2C
extern float pca_freq;
extern uint32_t pca_osc;
extern int pca_ticks[16];                     // giá trị off đang ghi ở mỗi kênh
extern int pca_on[16];                        // giá trị on đang ghi ở mỗi kênh
extern int pca_writes[16];                    // số lần ghi vào mỗi kênh

class Adafruit_PWMServoDriver {
public:
  Adafruit_PWMServoDriver(uint8_t = 0x40) {}
  bool begin() { return pca_present; }
  void setOscillatorFrequency(uint32_t f) { pca_osc = f; }
  void setPWMFreq(float f) { pca_freq = f; }
  uint8_t setPWM(uint8_t ch, uint16_t on, uint16_t off) {
    pca_on[ch] = on;
    pca_ticks[ch] = off;
    pca_writes[ch]++;
    return 0;
  }
};
