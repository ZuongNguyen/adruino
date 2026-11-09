// Chạy thử logic prototype 12 (bài 1.4): bấm nút (có rung tiếp điểm) từng ms, ghi lại góc 3 servo,
// kiểm tra góc quay tới của từng nút, nhịp đổi chiều STEP_MS và cách chuyển giữa hai nút.
// Chạy từ thư mục gốc dự án: bash tests/run_all.sh
#ifdef __AVR__   // Tinkercad, Wokwi, Arduino IDE biên dịch cho chip AVR
#error "Đây là file test chạy trên máy tính, không dán vào mạch. Hãy dán file prototype-12-3-servo-2-nut/sketch.ino"
#endif
#include "Arduino.h"
#include "Servo.h"
#include <string>
#include <vector>
#include "../prototype-12-3-servo-2-nut/sketch.ino"

const int RELEASED = (PRESSED == HIGH) ? LOW : HIGH;
const unsigned long BOUNCE_MS = 10;          // press() rung 5 lần × 2 ms trước khi giữ yên
bool ok = true;

// Mỗi ms một ký tự: '0' cả dãy ở REST_ANGLE, '1'/'2' đúng góc quay tới của nút 1/2, '?' góc lạ.
// trace[t] là trạng thái sau loop() tại millis() = t.
std::string trace;

char pattern() {
  for (int m = 0; m < BUTTON_COUNT; m++) {
    bool match = true;
    for (int i = 0; i < SERVO_COUNT; i++) if (servo_angle[i] != OUT_ANGLES[m][i]) match = false;
    if (match) return '1' + m;
  }
  for (int i = 0; i < SERVO_COUNT; i++) if (servo_angle[i] != REST_ANGLE) return '?';
  return '0';
}

void run(unsigned long ms) {
  for (unsigned long k = 0; k < ms; k++) { loop(); trace += pattern(); fake_ms++; }
}

// Nhấn nút b: rung 5 lần trong 10 ms, giữ holdMs, nhả cũng rung rồi chờ. Trả về lúc bắt đầu nhấn.
unsigned long press(uint8_t b, unsigned long holdMs) {
  unsigned long t0 = fake_ms;
  uint8_t p = BUTTON_PINS[b];
  for (int k = 0; k < 5; k++) {
    fake_pin[p] = PRESSED;  run(1);
    fake_pin[p] = RELEASED; run(1);
  }
  fake_pin[p] = PRESSED; run(holdMs);
  for (int k = 0; k < 5; k++) {
    fake_pin[p] = RELEASED; run(1);
    fake_pin[p] = PRESSED;  run(1);
  }
  fake_pin[p] = RELEASED; run(3 * DEBOUNCE_MS);
  return t0;
}

// Chạy tới lúc nếu nhấn nút thì nút được nhận đúng tại vị trí `phase` trong vòng (vòng bắt đầu ở roundAt)
void waitPhase(unsigned long roundAt, unsigned long acceptDelay, unsigned long phase) {
  while ((fake_ms + acceptDelay - roundAt) % ROUND_MS != phase) run(1);
}

// Vị trí đầu tiên từ `from` mà trace khác ký tự c
unsigned long firstNot(unsigned long from, char c) {
  while (from < trace.size() && trace[from] == c) from++;
  return from;
}

void check(const char *what, bool cond) {
  printf("%-56s %s\n", what, cond ? "đúng" : "SAI");
  if (!cond) ok = false;
}

int main() {
  printf("STEP_MS=%lu, ROUND_MS=%lu\n", STEP_MS, ROUND_MS);
  for (int m = 0; m < BUTTON_COUNT; m++) {
    printf("Nút %d:", m + 1);
    for (int i = 0; i < SERVO_COUNT; i++) printf(" servo %d -> %d°", i + 1, OUT_ANGLES[m][i]);
    printf("\n");
  }

  for (uint8_t b = 0; b < BUTTON_COUNT; b++) fake_pin[BUTTON_PINS[b]] = RELEASED;
  fake_ms = 0;
  setup();
  int setupWrites = servo_writes;
  run(1000);
  check("Bật nguồn: cả 3 servo đứng yên ở 0°", trace.find_first_not_of('0') == std::string::npos);
  check("Không gửi lệnh servo khi đứng yên", servo_writes == setupWrites);

  // 1. Nút 1 khi đang đứng yên: quay tới ngay sau thời gian lọc nhiễu
  unsigned long p1 = press(0, 200);
  unsigned long s1 = firstNot(p1, '0');
  unsigned long acceptDelay = s1 - p1;
  printf("Nút được nhận sau %lu ms kể từ lúc chạm\n", acceptDelay);
  check("Nút 1 khi đứng yên -> góc nút 1", trace[s1] == '1');
  check("Phản hồi trong thời gian lọc nhiễu", acceptDelay <= BOUNCE_MS + DEBOUNCE_MS + 2);
  run(3 * ROUND_MS);

  // 2. Giữ nút 1 lâu và nhấn lại nút 1: không lệch nhịp (kiểm tra ở phần nhịp bên dưới)
  press(0, 2000);
  run(ROUND_MS);
  press(0, 100);
  run(ROUND_MS);

  // 3. Đổi sang nút 2 giữa lúc servo đang ở góc quay tới: về 0° ngay, đứng đủ STEP_MS rồi quay góc nút 2
  waitPhase(s1, acceptDelay, STEP_MS / 2);
  unsigned long p2 = press(1, 200);
  unsigned long a2 = p2 + acceptDelay;
  check("Đổi sang nút 2 khi đang quay tới: về 0° ngay",
        trace[a2 - 1] == '1' && trace[a2] == '0');
  run(ROUND_MS);                                           // chạy đủ để thấy lần quay tới kế tiếp
  unsigned long s2 = firstNot(a2, '0');
  check("Đứng ở 0° đủ STEP_MS rồi quay tới góc nút 2",
        s2 - a2 == STEP_MS && trace[s2] == '2');
  run(2 * ROUND_MS);

  // 4. Đổi về nút 1 giữa lúc servo đang ở 0°: đứng nốt cho đủ STEP_MS như nhịp cũ rồi quay góc nút 1
  waitPhase(s2, acceptDelay, STEP_MS + STEP_MS / 2);
  unsigned long p3 = press(0, 200);
  unsigned long a3 = p3 + acceptDelay;
  run(ROUND_MS);
  unsigned long restStart = a3;
  while (trace[restStart - 1] == '0') restStart--;
  unsigned long s3 = firstNot(a3, '0');
  check("Đổi sang nút 1 khi đang ở 0°: giữ nhịp, quay góc nút 1",
        s3 - restStart == STEP_MS && trace[s3] == '1' && trace[restStart - 1] == '2');

  // 5. Nhiễu ngắn hơn thời gian lọc trên nút 2 thì bỏ qua
  fake_pin[BUTTON_PINS[1]] = PRESSED;  run(DEBOUNCE_MS / 2);
  fake_pin[BUTTON_PINS[1]] = RELEASED; run(2 * ROUND_MS);
  check("Nhiễu nút 2 ngắn hơn DEBOUNCE_MS: vẫn chạy nút 1",
        trace.find('2', s3) == std::string::npos);

  // Nhịp trên toàn bộ quá trình: tách trace thành các đoạn giữ nguyên một trạng thái
  std::vector<char> seg;
  std::vector<unsigned long> len;
  for (unsigned long t = s1; t < trace.size(); t++) {
    if (seg.empty() || trace[t] != seg.back()) { seg.push_back(trace[t]); len.push_back(0); }
    len.back()++;
  }
  bool noOdd = trace.find('?') == std::string::npos;
  bool restOk = true, outOk = true, alternate = true;
  int cut = 0;
  for (size_t k = 0; k + 1 < seg.size(); k++) {          // bỏ đoạn cuối vì bị cắt khi dừng test
    if (k > 0 && (seg[k] == '0') == (seg[k - 1] == '0')) alternate = false;
    if (seg[k] == '0') {
      if (len[k] != STEP_MS) restOk = false;             // luôn đứng ở 0° đủ STEP_MS
    } else if (len[k] != STEP_MS) {
      // Đoạn quay tới chỉ được ngắn hơn khi bị đổi sang nút khác
      bool switched = k + 2 < seg.size() && seg[k + 2] != seg[k];
      if (len[k] > STEP_MS || !switched) outOk = false;
      cut++;
    }
  }
  printf("Số đoạn: %d, đoạn quay tới bị cắt do đổi nút: %d\n", (int)seg.size(), cut);
  check("Không có góc lạ (chỉ 0° hoặc đúng góc của nút)", noOdd);
  check("Quay tới và quay về xen kẽ", alternate);
  check("Mỗi lần ở 0° kéo dài đúng STEP_MS", restOk);
  check("Mỗi lần ở góc quay tới kéo dài đúng STEP_MS (trừ lúc đổi nút)", outOk && cut == 1);

  printf(ok ? "KẾT QUẢ: ĐẠT\n" : "KẾT QUẢ: KHÔNG ĐẠT\n");
  return ok ? 0 : 1;
}
