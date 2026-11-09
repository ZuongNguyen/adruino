// Chạy thử logic prototype 14 trong 2 chu kỳ, mỗi bước 1 ms.
// Chạy từ thư mục gốc dự án: bash tests/run_all.sh
#ifdef __AVR__   // Tinkercad, Wokwi, Arduino IDE biên dịch cho chip AVR
#error "Đây là file test chạy trên máy tính, không dán vào mạch. Hãy dán file prototype-14-mach-thu-nghiem-1-servo-pca-9685/sketch.ino"
#endif
#include "Arduino.h"
#include "Adafruit_PWMServoDriver.h"
#include <stdlib.h>
#include <vector>
#include "../prototype-14-mach-thu-nghiem-1-servo-pca-9685/sketch.ino"

static bool ok = true;

static void expect(const char* name, bool pass) {
  printf("%s: %s\n", name, pass ? "đúng" : "SAI");
  if (!pass) ok = false;
}

int main() {
  printf("SHOW_MS=%lu  QUET_ROUND=%lu  NAC_ROUND=%lu\n", SHOW_MS, QUET_ROUND_MS, NAC_ROUND_MS);
  setup();
  expect("Tần số PWM 50 Hz", pca_freq == (float)PWM_HZ);
  expect("Đặt tần số dao động", pca_osc == PCA_OSC_HZ);
  expect("Khởi động ở 0°", pca_ticks[SERVO_CHANNEL] == pulseTicks(0));
  const int setupWrites = pca_writes[SERVO_CHANNEL];

  const unsigned long T = 2 * SHOW_MS;
  std::vector<int> h(T + 1);
  for (fake_ms = 0; fake_ms <= T; fake_ms++) {
    loop();
    h[fake_ms] = pca_ticks[SERVO_CHANNEL];
  }

  bool otherChannels = true;                          // chỉ kênh đã chọn được ghi, xung bật ở đầu chu kỳ
  for (int c = 0; c < 16; c++) if (c != SERVO_CHANNEL && pca_writes[c] != 0) otherChannels = false;
  expect("Chỉ ghi vào đúng kênh", otherChannels);
  expect("Xung bật từ bước 0", pca_on[SERVO_CHANNEL] == 0);

  int lo = h[0], hi = h[0];                           // đi đủ hai đầu, không vượt giới hạn
  for (unsigned long t = 0; t <= T; t++) { if (h[t] < lo) lo = h[t]; if (h[t] > hi) hi = h[t]; }
  printf("Bước xung: %d..%d\n", lo, hi);
  expect("Đi đủ 0° và 180°", lo == pulseTicks(0) && hi == pulseTicks(MAX_ANGLE));

  // Độ rộng xung phải nằm trong dải an toàn của servo
  const long minUs = (long)lo * 1000000L / (PCA_STEPS * PWM_HZ);
  const long maxUs = (long)hi * 1000000L / (PCA_STEPS * PWM_HZ);
  printf("Độ rộng xung: %ld..%ld us\n", minUs, maxUs);
  expect("Xung trong dải 500..2500 us", minUs >= 500 && maxUs <= 2500);

  int maxStep = 0;                                    // không có cú nhảy, ngưỡng là 1° mỗi ms
  long changes = 0;
  for (unsigned long t = 1; t <= T; t++) {
    int d = abs(h[t] - h[t - 1]);
    if (d > maxStep) maxStep = d;
    if (d != 0) changes++;
  }
  const int stepLimit = pulseTicks(1) - pulseTicks(0) + 1;
  printf("Xung đổi tối đa mỗi ms: %d bước (ngưỡng %d)\n", maxStep, stepLimit);
  expect("Không có cú nhảy", maxStep <= stepLimit);
  expect("Chỉ ghi khi góc đổi", pca_writes[SERVO_CHANNEL] - setupWrites == changes);

  bool zero = true;                                   // mỗi vòng bắt đầu ở 0°
  for (unsigned long base = 0; base <= T; base += SHOW_MS) {
    std::vector<unsigned long> starts;
    for (unsigned long r = 0; r < QUET_ROUNDS; r++) starts.push_back(base + r * QUET_ROUND_MS);
    for (unsigned long r = 0; r < NAC_ROUNDS; r++) starts.push_back(base + QUET_TOTAL_MS + r * NAC_ROUND_MS);
    for (size_t k = 0; k < starts.size(); k++)
      if (starts[k] <= T && h[starts[k]] != pulseTicks(0)) zero = false;
  }
  expect("Mọi vòng bắt đầu ở 0°", zero);

  bool steps = true;                                  // đứng đúng góc ở cuối mỗi nấc, cả lượt lên và lượt về
  for (unsigned long n = 1; n <= NAC_COUNT; n++) {
    int a = MAX_ANGLE * n / NAC_COUNT;
    if (h[QUET_TOTAL_MS + n * NAC_STEP_MS - 1] != pulseTicks(a)) steps = false;
    if (h[QUET_TOTAL_MS + NAC_HALF_MS + n * NAC_STEP_MS - 1] != pulseTicks(MAX_ANGLE - a)) steps = false;
  }
  expect("Đứng đúng góc ở từng nấc", steps);

  long minPause = 1L << 30;                           // luôn dừng trước khi đổi chiều
  int dir = 0; long still = 0;
  for (unsigned long t = 1; t <= T; t++) {
    int d = h[t] - h[t - 1];
    if (d == 0) { still++; continue; }
    int nd = d > 0 ? 1 : -1;
    if (dir != 0 && nd != dir && still < minPause) minPause = still;
    dir = nd; still = 0;
  }
  const long needPause = QUET_HOLD_MS < NAC_HOLD_MS ? QUET_HOLD_MS : NAC_HOLD_MS;
  printf("Dừng ít nhất trước khi đổi chiều: %ld ms (cần %ld)\n", minPause, needPause);
  expect("Dừng trước khi đổi chiều", minPause >= needPause);

  // Không thấy PCA9685: không ghi kênh nào, đèn L nháy đúng nhịp
  pca_present = false;
  memset(pca_writes, 0, sizeof(pca_writes));
  fake_ms = 0;
  setup();
  loop();
  int blinks = 0, prev = fake_pin[ERROR_LED_PIN];
  const unsigned long blinkT = 20 * ERROR_BLINK_MS;
  for (fake_ms = 1; fake_ms <= blinkT; fake_ms++) {
    loop();
    if (fake_pin[ERROR_LED_PIN] != prev) { blinks++; prev = fake_pin[ERROR_LED_PIN]; }
  }
  printf("Đèn L đổi trạng thái %d lần trong %lu ms\n", blinks, blinkT);
  expect("Báo lỗi khi không thấy chip", blinks == 20 && pca_writes[SERVO_CHANNEL] == 0);

  printf(ok ? "KẾT QUẢ: ĐẠT\n" : "KẾT QUẢ: KHÔNG ĐẠT\n");
  return ok ? 0 : 1;
}
