// Động tác gắp đồ: đặt 4 servo về góc ban đầu, rồi chạy bước 1 lần lượt từng servo – Arduino Uno
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
//   1. Đặt ban đầu: servo 1 = 0°, servo 2 = 30°, servo 3 = 80°, servo 4 = 20° (GOC_BAN_DAU[]).
//      Lần lượt từng servo, cách nhau SET_GAP_MS, để không cùng kéo dòng một lúc (dễ sụt nguồn, Arduino tự reset).
//      Bước này servo quay hết tốc độ, vì chưa biết lúc đầu servo đang đứng ở góc nào để quay chậm từ đó.
//      Servo chưa tới lượt thì chưa attach(): chân chưa có xung nên servo đứng yên chỗ cũ.
//   2. Chờ SET_HOLD_MS rồi chạy bảng BUOC_1[] từ trên xuống, mỗi lúc chỉ một servo quay, cùng tốc độ MOVE_SPEED_DEG_S:
//      servo 2: 30° -> 45°; servo 3: 80° -> 60°; servo 4: 20° -> 60° -> 20°; servo 1: 0° -> 45° -> 90° -> 45°.
//      Chuyển sang servo khác thì nghỉ NGHI_DOI_SERVO_MS (1,5 s); cùng servo đi tiếp thì dừng NGHI_CUNG_SERVO_MS (0,5 s).
//   3. Xong bước 1 thì cả 4 servo đứng yên ở góc cuối.
// Với thông số mặc định: đặt ban đầu 0–1,5 s; servo 2 quay 3,0–4,0 s; servo 3 5,5–6,8 s; servo 4 8,3–11,0 s và 11,5–14,2 s;
// servo 1 15,7–18,7 s, 19,2–22,2 s, 22,7–25,7 s. Sau 25,7 s đứng yên.

#include <Servo.h>

const uint8_t SERVO_PINS[]  = {5, 6, 9, 10};      // servo 1 -> 4 theo quy ước của người dùng (bỏ servo 5)
const int     GOC_BAN_DAU[] = {0, 30, 80, 20};    // góc đặt ban đầu của servo 1 -> 4, cùng thứ tự SERVO_PINS[]
const uint8_t SERVO_COUNT   = sizeof(SERVO_PINS) / sizeof(SERVO_PINS[0]);

// Bước 1, chạy lần lượt từ trên xuống: { số servo của người dùng (1 -> 4), góc quay tới }.
// Góc xuất phát của mỗi dòng là góc servo đó đang đứng (góc ban đầu hoặc góc đích của dòng trước của nó).
struct BuocQuay {
  uint8_t servo;
  int goc;
};
const BuocQuay BUOC_1[] = {
  {2, 45},   // servo 2: 30° -> 45°
  {3, 60},   // servo 3: 80° -> 60°
  {4, 60},   // servo 4: 20° -> 60°
  {4, 20},   //          rồi về lại 20°
  {1, 45},   // servo 1: 0° -> 45°
  {1, 90},   //          rồi lên 90°
  {1, 45},   //          rồi quay ngược về 45°
};
const uint8_t BUOC_COUNT = sizeof(BUOC_1) / sizeof(BUOC_1[0]);

// ---- Thông số ----
const unsigned long SET_GAP_MS         = 500;   // ms: servo sau về góc ban đầu muộn hơn servo trước bấy nhiêu (0 = cùng lúc)
const unsigned long SET_HOLD_MS        = 1500;  // ms: chờ sau khi servo cuối nhận góc ban đầu, rồi mới bắt đầu bước 1
const unsigned long MOVE_SPEED_DEG_S   = 15;    // tốc độ quay, độ mỗi giây: càng nhỏ càng chậm (0 = hết tốc độ)
const unsigned long NGHI_DOI_SERVO_MS  = 1500;  // ms: nghỉ khi chuyển từ servo này sang servo khác
const unsigned long NGHI_CUNG_SERVO_MS = 500;   // ms: dừng khi cùng một servo đi tiếp hoặc đổi chiều

Servo servos[SERVO_COUNT];
bool daDat[SERVO_COUNT];                   // servo đã nhận góc ban đầu (đã attach) chưa
int lastAngle[SERVO_COUNT];                // góc đã ghi gần nhất, chỉ ghi lại khi góc đổi
int buocTu[BUOC_COUNT];                    // góc xuất phát của từng dòng trong BUOC_1[]
unsigned long buocBatDau[BUOC_COUNT];      // lúc bắt đầu quay của từng dòng, tính từ đầu bước 1
unsigned long buocThoiGian[BUOC_COUNT];    // thời gian quay của từng dòng (ms), tính từ số độ và tốc độ
unsigned long buocKetThuc = 0;             // lúc dòng cuối quay xong
bool xong = false;                         // đã chạy xong bước 1, loop() không còn việc gì
unsigned long mocKhoiDong = 0;             // millis() lúc setup()

// Góc của dòng thứ k trong BUOC_1[] tại thời điểm t (tính từ đầu bước 1), khi dòng đó đã bắt đầu
int buocAngleAt(uint8_t k, unsigned long t) {
  unsigned long daQuay = t - buocBatDau[k];
  if (daQuay >= buocThoiGian[k]) return BUOC_1[k].goc;     // đã quay xong (thời gian 0 thì tới luôn)
  return buocTu[k] + (long)(BUOC_1[k].goc - buocTu[k]) * (long)daQuay / (long)buocThoiGian[k];
}

void setup() {
  int gocHienTai[SERVO_COUNT];
  for (uint8_t i = 0; i < SERVO_COUNT; i++) {
    // Ghi góc TRƯỚC khi attach(): xung đầu tiên đã là góc ban đầu, servo không bị kéo qua 90° mặc định của thư viện
    servos[i].write(GOC_BAN_DAU[i]);
    lastAngle[i]  = GOC_BAN_DAU[i];
    gocHienTai[i] = GOC_BAN_DAU[i];
    daDat[i] = false;
  }

  // Lập lịch bước 1: mỗi dòng bắt đầu khi dòng trước quay xong và nghỉ đủ
  unsigned long moc = 0;
  uint8_t servoTruoc = 0;                  // 0 = chưa có dòng nào trước
  for (uint8_t k = 0; k < BUOC_COUNT; k++) {
    uint8_t i = BUOC_1[k].servo - 1;       // số servo 1 -> 4 đổi thành chỉ số mảng 0 -> 3
    if (i >= SERVO_COUNT) {                // ghi nhầm số servo (0 hoặc lớn hơn 4): bỏ dòng này
      buocTu[k] = BUOC_1[k].goc;
      buocBatDau[k] = moc;
      buocThoiGian[k] = 0;
      continue;
    }
    if (servoTruoc != 0) moc += (BUOC_1[k].servo == servoTruoc) ? NGHI_CUNG_SERVO_MS : NGHI_DOI_SERVO_MS;
    int doQuay = BUOC_1[k].goc - gocHienTai[i];
    if (doQuay < 0) doQuay = -doQuay;
    buocTu[k]       = gocHienTai[i];
    buocBatDau[k]   = moc;
    buocThoiGian[k] = MOVE_SPEED_DEG_S > 0 ? (unsigned long)doQuay * 1000UL / MOVE_SPEED_DEG_S : 0;
    moc += buocThoiGian[k];
    buocKetThuc = moc;
    gocHienTai[i] = BUOC_1[k].goc;
    servoTruoc = BUOC_1[k].servo;
  }

  mocKhoiDong = millis();
}

void loop() {
  if (xong) return;
  unsigned long daQua = millis() - mocKhoiDong;

  // 1. Đặt ban đầu: tới lượt servo thứ i (tính từ 0) thì attach(), servo nhận xung góc ban đầu và quay tới đó
  for (uint8_t i = 0; i < SERVO_COUNT; i++) {
    if (!daDat[i] && daQua >= (unsigned long)i * SET_GAP_MS) {
      servos[i].attach(SERVO_PINS[i]);
      daDat[i] = true;
    }
  }

  // 2. Bước 1, bắt đầu sau khi servo cuối nhận góc ban đầu được SET_HOLD_MS
  unsigned long moveStart = (unsigned long)(SERVO_COUNT - 1) * SET_GAP_MS + SET_HOLD_MS;
  if (daQua < moveStart) return;
  unsigned long t = daQua - moveStart;

  for (uint8_t i = 0; i < SERVO_COUNT; i++) {
    // Góc của servo i = góc của dòng mới nhất (của servo này) đã bắt đầu; chưa dòng nào bắt đầu thì giữ góc ban đầu
    int angle = GOC_BAN_DAU[i];
    for (uint8_t k = 0; k < BUOC_COUNT; k++) {
      if (BUOC_1[k].servo != i + 1) continue;
      if (t < buocBatDau[k]) break;        // các dòng xếp theo thời gian, dòng này chưa tới thì dòng sau cũng chưa
      angle = buocAngleAt(k, t);
    }
    if (angle != lastAngle[i]) {
      servos[i].write(angle);
      lastAngle[i] = angle;
    }
  }

  // Dòng cuối đã quay xong (góc cuối đã ghi ở vòng for trên) thì dừng hẳn
  if (t >= buocKetThuc) xong = true;
}
