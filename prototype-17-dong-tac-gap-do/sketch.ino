// Động tác gắp đồ (động tác 1): đưa 4 servo về góc base, chạy bước 1 -> 3, rồi về lại góc base – Arduino Uno
// Mạch: mạch Tinkercad 5 servo của bản 16 (servo xếp dọc bên phải Arduino), BỎ servo 5. Quy ước của người dùng:
// servo 1 → 5 theo chân tín hiệu 5, 6, 9, 10, 11, nên servo 1 là servo DƯỚI CÙNG trong ảnh:
//   Servo 1 (dưới cùng) dây tín hiệu -> chân 5   (dây cam)
//   Servo 2                          -> chân 6   (dây xanh lá)
//   Servo 3 (giữa)                   -> chân 9   (dây xanh nhạt)
//   Servo 4                          -> chân 10  (dây tím)
//   Servo 5 (trên cùng, chân 11)     -> KHÔNG dùng: chương trình không điều khiển, tháo ra hay để nguyên đều được
//   Dây đỏ của servo  -> hàng nguồn + dưới breadboard (nối 5V Arduino)
//   Dây nâu của servo -> hàng nguồn - dưới breadboard (nối GND Arduino)
//   KHÔNG cắm dây đỏ vào Vin hay 3.3V. Mạch thật 4 servo cần nguồn 5V riêng, nối chung GND (xem README)
//
// Hiệu ứng, mỗi lần cấp điện hoặc nhấn nút RESET đỏ trên Arduino:
//   1. Góc base: servo 1 = 90° (vị trí giữa, để quay được cả trái lẫn phải), servo 2 = 15°, servo 3 = 30°, servo 4 = 0°.
//      Lần lượt từng servo, cách nhau SET_GAP_MS, để không cùng kéo dòng một lúc (dễ sụt nguồn, Arduino tự reset).
//      Bước này servo quay hết tốc độ, vì chưa biết lúc đầu servo đang đứng ở góc nào để quay chậm từ đó.
//      Servo chưa tới lượt thì chưa attach(): chân chưa có xung, chương trình chưa điều khiển servo đó.
//   2. Chờ SET_HOLD_MS rồi chạy bảng DONG_TAC_1[] từ trên xuống. Servo quay đều theo bảng tốc độ TOC_DO_DEG_S[][] (theo bước
//      và servo): bước 1 servo 1, 2, 3: 50°/s, servo 4: 60°/s; bước 2, 3 và lúc về base servo 1: 30°/s, servo 2, 3: 20°/s,
//      servo 4: 40°/s.
//      Bước 1, lần lượt: servo 3: 30° -> 60°; servo 2: 15° -> 45°;
//              servo 1: 90° -> 135° (trái 45°) -> 90° -> 45° (phải 45°) -> 90°; servo 4: 0° -> 180° -> 0°.
//      Bước 2, cùng lúc: servo 2: 45° -> 130° và servo 3: 60° -> 120°.
//      Bước 3, servo 2, 3 giữ 130°/120° của bước 2: servo 4: 0° -> 120° rồi giữ;
//              servo 2, 3 cùng lúc về góc của bước 1 (45°/60°); servo 1: 90° -> 180° (trái 90°) rồi giữ;
//              servo 2, 3 cùng lúc làm lại bước 2 (130°/120°); servo 4: 120° -> 0°; servo 2, 3 cùng lúc về 45°/60°.
//      Dòng LAN_LUOT chờ mọi dòng trên quay xong rồi nghỉ: NGHI_DOI_SERVO_MS (1,5 s) nếu đổi servo,
//      NGHI_CUNG_SERVO_MS (0,5 s) nếu cùng servo đi tiếp/đổi chiều. Dòng CUNG_LUC bắt đầu cùng lúc với dòng ngay trên.
//   3. Về base: nghỉ NGHI_DOI_SERVO_MS rồi mọi servo còn lệch góc base cùng quay về base. setup() tự thêm phần này,
//      nên sửa bảng thế nào thì cuối cùng servo vẫn về base. Sau đó đứng yên.
// Với thông số mặc định (giây, tính từ lúc khởi động): góc base 0–1,5;
//   bước 1: servo 3 3,0–3,6; servo 2 5,1–5,7; servo 1 7,2–8,1, 8,6–9,5, 10,0–10,9, 11,4–12,3; servo 4 13,8–16,8 và 17,3–20,3;
//   bước 2: servo 2 21,8–26,05 và servo 3 21,8–24,8;
//   bước 3: servo 4 27,55–30,55; servo 2, 3 32,05–36,3; servo 1 37,8–40,8; servo 2, 3 42,3–46,55; servo 4 48,05–51,05;
//           servo 2, 3 52,55–56,8;
//   về base: servo 1 180° -> 90° 58,3–61,3; servo 2 45° -> 15° và servo 3 60° -> 30° 58,3–59,8 (servo 4 đã ở 0°).
//   Sau 61,3 s đứng yên.

#include <Servo.h>

const uint8_t SERVO_PINS[] = {5, 6, 9, 10};     // servo 1 -> 4 theo quy ước của người dùng (bỏ servo 5)
const int     GOC_BASE[]   = {90, 15, 30, 0};   // góc base của servo 1 -> 4, cùng thứ tự SERVO_PINS[]
const uint8_t SERVO_COUNT  = sizeof(SERVO_PINS) / sizeof(SERVO_PINS[0]);

// ---- Thông số ----
// Tốc độ quay, độ mỗi giây, theo bước (hàng) và servo (cột): càng nhỏ càng chậm, 0 = hết tốc độ.
// Hàng thứ n là bước n (cột "bước" trong DONG_TAC_1[]); hàng CUỐI là lúc về base. Thêm bước thì thêm hàng ngay trên hàng về base.
const unsigned long TOC_DO_DEG_S[][SERVO_COUNT] = {
  // servo 1, 2,  3,  4
  {50, 50, 50, 60},   // bước 1
  {30, 20, 20, 40},   // bước 2
  {30, 20, 20, 40},   // bước 3
  {30, 20, 20, 40},   // về base
};
const uint8_t HANG_VE_BASE = sizeof(TOC_DO_DEG_S) / sizeof(TOC_DO_DEG_S[0]) - 1;   // chỉ số hàng về base (= số bước có tốc độ)

const unsigned long SET_GAP_MS        = 500;   // ms: servo sau về góc base muộn hơn servo trước bấy nhiêu (0 = cùng lúc)
const unsigned long SET_HOLD_MS        = 1500;  // ms: chờ sau khi servo cuối nhận góc base, rồi mới bắt đầu bước 1
const unsigned long NGHI_DOI_SERVO_MS  = 1500;  // ms: nghỉ khi chuyển sang servo khác, và trước khi về base
const unsigned long NGHI_CUNG_SERVO_MS = 500;   // ms: dừng khi cùng một servo đi tiếp hoặc đổi chiều

// Các bước của động tác, chạy từ trên xuống: { bước, số servo của người dùng (1 -> 4), góc quay tới, LAN_LUOT hoặc CUNG_LUC }.
// Cột "bước" chỉ dùng để chọn hàng tốc độ trong TOC_DO_DEG_S[][]; ghi số bước chưa có hàng tốc độ thì dòng đó bị bỏ.
// Góc xuất phát của mỗi dòng là góc servo đó đang đứng (góc base hoặc góc đích của dòng trước của nó).
// LAN_LUOT: chờ các dòng trên quay xong hết, nghỉ, rồi mới quay. CUNG_LUC: bắt đầu cùng lúc với dòng ngay trên.
// Trong một nhóm CUNG_LUC mỗi servo chỉ ghi một dòng (ghi hai lần thì dòng sau tự chờ như LAN_LUOT).
// KHÔNG cần ghi dòng về base ở cuối: setup() tự thêm.
// Servo 1: coi tăng góc là quay sang TRÁI. Nếu trên mạch thấy ngược bên thì đổi 135 và 45 cho nhau, và 180 ở bước 3 thành 0.
struct BuocQuay {
  uint8_t buoc;
  uint8_t servo;
  int goc;
  bool cungLuc;
};
const bool LAN_LUOT = false;
const bool CUNG_LUC = true;
const BuocQuay DONG_TAC_1[] = {
  // Bước 1: lần lượt từng servo
  {1, 3, 60,  LAN_LUOT},   // 1. servo 3: 30° -> 60°
  {1, 2, 45,  LAN_LUOT},   // 2. servo 2: 15° -> 45°
  {1, 1, 135, LAN_LUOT},   // 3. servo 1: 90° -> 135° (sang trái 45°)
  {1, 1, 90,  LAN_LUOT},   //             về giữa 90°
  {1, 1, 45,  LAN_LUOT},   //             sang phải 45° (45°)
  {1, 1, 90,  LAN_LUOT},   //             về giữa 90°
  {1, 4, 180, LAN_LUOT},   // 4. servo 4: 0° -> 180°
  {1, 4, 0,   LAN_LUOT},   //             rồi về 0°
  // Bước 2: servo 2 và servo 3 cùng lúc
  {2, 2, 130, LAN_LUOT},   // servo 2: 45° -> 130°
  {2, 3, 120, CUNG_LUC},   // servo 3: 60° -> 120°, bắt đầu cùng lúc với servo 2
  // Bước 3: servo 2, 3 vẫn giữ 130°/120° của bước 2
  {3, 4, 120, LAN_LUOT},   // 1. servo 4: 0° -> 120° rồi giữ
  {3, 2, 45,  LAN_LUOT},   // 2. servo 2, 3 cùng lúc về góc của bước 1: servo 2 130° -> 45°
  {3, 3, 60,  CUNG_LUC},   //                                        servo 3 120° -> 60°
  {3, 1, 180, LAN_LUOT},   // 3. servo 1: 90° -> 180° (sang trái 90°) rồi giữ
  {3, 2, 130, LAN_LUOT},   // 4. servo 2, 3 cùng lúc làm lại bước 2: servo 2 45° -> 130°
  {3, 3, 120, CUNG_LUC},   //                                        servo 3 60° -> 120°
  {3, 4, 0,   LAN_LUOT},   // 5. servo 4: 120° -> 0°
  {3, 2, 45,  LAN_LUOT},   // 6. servo 2, 3 cùng lúc về góc của bước 1: servo 2 130° -> 45°
  {3, 3, 60,  CUNG_LUC},   //                                        servo 3 120° -> 60°
  // Hết bước 3: setup() tự thêm phần về base (servo 1 180° -> 90°, servo 2 -> 15°, servo 3 -> 30°)
};
const uint8_t BUOC_COUNT = sizeof(DONG_TAC_1) / sizeof(DONG_TAC_1[0]);
const uint8_t LICH_MAX   = BUOC_COUNT + SERVO_COUNT;   // các dòng của DONG_TAC_1[] + tối đa 1 dòng về base mỗi servo

Servo servos[SERVO_COUNT];
bool daDat[SERVO_COUNT];                   // servo đã nhận góc base (đã attach) chưa
int lastAngle[SERVO_COUNT];                // góc đã ghi gần nhất, chỉ ghi lại khi góc đổi
int gocSau[SERVO_COUNT];                   // khi lập lịch: góc servo đứng sau các dòng đã xếp
uint8_t lichServo[LICH_MAX];               // lịch quay lập trong setup(): chỉ số servo (0 -> 3) của từng dòng
int lichTu[LICH_MAX];                      //   góc xuất phát
int lichDen[LICH_MAX];                     //   góc đích
unsigned long lichBatDau[LICH_MAX];        //   lúc bắt đầu quay, tính từ đầu bước 1
unsigned long lichThoiGian[LICH_MAX];      //   thời gian quay (ms), tính từ số độ và tốc độ của servo đó
uint8_t lichCount = 0;                     // số dòng đã có trong lịch
unsigned long lichKetThuc = 0;             // lúc dòng quay xong muộn nhất (dòng về base cuối cùng)
bool xong = false;                         // đã về base xong, loop() không còn việc gì
unsigned long mocKhoiDong = 0;             // millis() lúc setup()

// Thêm một dòng vào lịch: servo i quay từ góc đang đứng tới góc den, bắt đầu lúc batDau, tốc độ tocDo (độ/giây, 0 = hết tốc độ)
void themLich(uint8_t i, int den, unsigned long batDau, unsigned long tocDo) {
  if (lichCount >= LICH_MAX) return;
  int doQuay = den - gocSau[i];
  if (doQuay < 0) doQuay = -doQuay;
  lichServo[lichCount]    = i;
  lichTu[lichCount]       = gocSau[i];
  lichDen[lichCount]      = den;
  lichBatDau[lichCount]   = batDau;
  lichThoiGian[lichCount] = tocDo > 0 ? (unsigned long)doQuay * 1000UL / tocDo : 0;
  unsigned long xongDong = batDau + lichThoiGian[lichCount];
  if (xongDong > lichKetThuc) lichKetThuc = xongDong;
  gocSau[i] = den;
  lichCount++;
}

// Góc của dòng thứ k trong lịch tại thời điểm t (tính từ đầu bước 1), khi dòng đó đã bắt đầu
int lichAngleAt(uint8_t k, unsigned long t) {
  unsigned long daQuay = t - lichBatDau[k];
  if (daQuay >= lichThoiGian[k]) return lichDen[k];        // đã quay xong (thời gian 0 thì tới luôn)
  return lichTu[k] + (long)(lichDen[k] - lichTu[k]) * (long)daQuay / (long)lichThoiGian[k];
}

void setup() {
  for (uint8_t i = 0; i < SERVO_COUNT; i++) {
    // Ghi góc TRƯỚC khi attach(): xung đầu tiên đã là góc base, servo không bị kéo qua 90° mặc định của thư viện
    servos[i].write(GOC_BASE[i]);
    lastAngle[i] = GOC_BASE[i];
    gocSau[i]    = GOC_BASE[i];
    daDat[i] = false;
  }

  // Lập lịch các bước. Nhóm = một dòng LAN_LUOT cùng các dòng CUNG_LUC ngay sau nó, cả nhóm bắt đầu cùng lúc.
  unsigned long batDauNhom = 0;
  uint8_t servoTruoc = 0;                  // số servo của dòng hợp lệ ngay trước, 0 = chưa có dòng nào
  uint8_t trongNhom  = 0;                  // bit i = servo i đã có dòng trong nhóm hiện tại
  for (uint8_t k = 0; k < BUOC_COUNT; k++) {
    uint8_t i = DONG_TAC_1[k].servo - 1;   // số servo 1 -> 4 đổi thành chỉ số mảng 0 -> 3
    if (i >= SERVO_COUNT) continue;        // ghi nhầm số servo (0 hoặc lớn hơn 4): bỏ dòng này
    uint8_t b = DONG_TAC_1[k].buoc;        // bước n dùng hàng n - 1 của TOC_DO_DEG_S[][]
    if (b == 0 || b > HANG_VE_BASE) continue;   // ghi nhầm số bước (0 hoặc chưa có hàng tốc độ): bỏ dòng này
    if (servoTruoc != 0 && (!DONG_TAC_1[k].cungLuc || (trongNhom & (1 << i)))) {
      // Nhóm mới: chờ mọi dòng trước quay xong, rồi nghỉ
      batDauNhom = lichKetThuc + (DONG_TAC_1[k].servo == servoTruoc ? NGHI_CUNG_SERVO_MS : NGHI_DOI_SERVO_MS);
      trongNhom = 0;
    }
    themLich(i, DONG_TAC_1[k].goc, batDauNhom, TOC_DO_DEG_S[b - 1][i]);
    trongNhom |= (1 << i);
    servoTruoc = DONG_TAC_1[k].servo;
  }

  // Về base: sau khi mọi dòng trên xong và nghỉ NGHI_DOI_SERVO_MS, servo nào còn lệch góc base thì cùng quay về
  batDauNhom = lichKetThuc + NGHI_DOI_SERVO_MS;
  for (uint8_t i = 0; i < SERVO_COUNT; i++) {
    if (gocSau[i] != GOC_BASE[i]) themLich(i, GOC_BASE[i], batDauNhom, TOC_DO_DEG_S[HANG_VE_BASE][i]);
  }

  mocKhoiDong = millis();
}

void loop() {
  if (xong) return;
  unsigned long daQua = millis() - mocKhoiDong;

  // 1. Góc base: tới lượt servo thứ i (tính từ 0) thì attach(), servo nhận xung góc base và quay tới đó
  for (uint8_t i = 0; i < SERVO_COUNT; i++) {
    if (!daDat[i] && daQua >= (unsigned long)i * SET_GAP_MS) {
      servos[i].attach(SERVO_PINS[i]);
      daDat[i] = true;
    }
  }

  // 2. Các bước rồi về base, bắt đầu sau khi servo cuối nhận góc base được SET_HOLD_MS
  unsigned long moveStart = (unsigned long)(SERVO_COUNT - 1) * SET_GAP_MS + SET_HOLD_MS;
  if (daQua < moveStart) return;
  unsigned long t = daQua - moveStart;

  for (uint8_t i = 0; i < SERVO_COUNT; i++) {
    // Góc của servo i = góc của dòng mới nhất (của servo này) đã bắt đầu; chưa dòng nào bắt đầu thì giữ góc base
    int angle = GOC_BASE[i];
    for (uint8_t k = 0; k < lichCount; k++) {
      if (lichServo[k] != i) continue;
      if (t < lichBatDau[k]) break;        // các dòng của một servo xếp theo thời gian, dòng này chưa tới thì dòng sau cũng chưa
      angle = lichAngleAt(k, t);
    }
    if (angle != lastAngle[i]) {
      servos[i].write(angle);
      lastAngle[i] = angle;
    }
  }

  // Dòng cuối đã quay xong (góc cuối đã ghi ở vòng for trên) thì dừng hẳn
  if (t >= lichKetThuc) xong = true;
}
