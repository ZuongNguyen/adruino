# Dự án adruino – Arduino Uno prototypes

Chuỗi prototype lập trình mạch **Arduino Uno** của Dai Duong, mô phỏng bằng **Wokwi** hoặc **Tinkercad**
(người dùng thường gửi ảnh mạch Tinkercad). Dự án chuyển từ Project "adruino" trên claude.ai sang đây
ngày 27/10/2026; bản gốc trên claude.ai vẫn còn.

Trả lời bằng **tiếng Việt**. Tài liệu chính cho người dùng là `README.md`.
Làm hoặc sửa prototype thì dùng skill **`arduino-prototype`** (`.claude/skills/arduino-prototype/SKILL.md`).

## Bắt buộc: hỏi tên dự án trước khi xuất file code

Người dùng yêu cầu ngày 09/11/2026, để không còn trùng lặp nhiều file sketch.

- Trước khi tạo file code mới (`.ino`, thư mục `prototype-NN-*`), **phải hỏi tên dự án và chờ trả lời**, kể cả khi
  đề bài và ảnh mạch đã rõ. Không tự đặt tên.
- So tên với các thư mục `prototype-*` đã có. Trùng hoặc gần giống (cùng mạch, cùng bài) thì hỏi: sửa bản cũ hay tạo bản mới.
- Sửa lỗi hoặc chỉnh một bản đã có thì **sửa ngay trên file đó**, không tạo bản sao.
- Hook `.claude/hooks/hoi-ten-du-an.py` bắt xác nhận mỗi khi sắp tạo một file `.ino` mới. Đây chỉ là lớp chặn
  cuối, vẫn phải hỏi tên trong hội thoại trước.

## Skill dùng cho dự án

| Việc | Skill |
|---|---|
| Làm bản mới, đổi chức năng | `arduino-prototype` |
| Sửa lỗi bản đã có, ngay trên file cũ | `arduino-sua-loi` |
| Tối ưu Flash/RAM, gọn code | `arduino-toi-uu` |
| Rà phần vừa sửa | `/simplify` (code lặp), `/code-review` (tìm lỗi) |
| Lỗi khó, nhiều bước | `engineering:debug` |

Đã xem và loại ngày 09/11/2026: `ESP32:esp32` (khác chip, chỉ dùng nếu chuyển sang ESP32); các skill công khai
`arduino-skills`, `embedded-c`, `avr`, `microcontroller-firmware`, `embedded-systems` (chung chung hoặc nhắm ARM/RTOS,
không có quy tắc Servo/Timer1, phần lớn không xem được nội dung thật, có skill kèm script chạy trên máy hoặc trượt
kiểm định bảo mật); plugin AI Skills, agentic-council, CodeRabbit (quá rộng so với dự án hoặc cần tài khoản riêng).

## Cấu trúc

```
README.md                      Bảng prototype, sơ đồ nối dây, logic chính, cách chạy, xử lý sự cố
prototype-NN-ten/sketch.ino    Mỗi prototype một thư mục, dán thẳng vào Wokwi/Tinkercad
prototype-02-.../libraries.txt Thư viện cho Wokwi (LiquidCrystal I2C)
tests/run_all.sh               Chạy toàn bộ 4 bước kiểm tra bên dưới
tests/check_pins.py            Kiểm tra chân: PWM, xung đột Servo/Timer1, trùng chân, A4/A5 khi có I2C
tests/compile_all.sh           Biên dịch thử mọi sketch bằng g++ với Arduino giả lập
tests/test_NN_*.cpp            Chạy thử logic theo từng ms (bản 05–13)
tests/compile_uno.sh           Biên dịch thật cho Uno bằng arduino-cli, in Flash/RAM
tests/mock/                    Arduino.h, Servo.h, Wire.h, LiquidCrystal_I2C.h giả lập + globals.cpp
build/                         Kết quả biên dịch (git bỏ qua)
```

Công cụ trên máy (cài 27/10/2026 bằng winget): **arduino-cli 1.5.1** (`C:\Program Files\Arduino CLI`, core
`arduino:avr`, thư viện `Servo`, `LiquidCrystal I2C`) và **g++ 16.2** (WinLibs). Shell mở trước khi cài có thể
chưa có g++ trong PATH, `run_all.sh` tự tìm. Dự án dùng git, nhánh `main`.

## Tiến trình

| # | Nội dung | Trạng thái |
|---|---|---|
| 01 | LED bật/tắt bằng nút, chống dội 50 ms | Xong |
| 02 | Như 01 + LCD 16x2 I2C hiện " Dai Duong " và trạng thái LED | Xong |
| 03 | 1 servo quay 0° ↔ 180°, dừng 0,5 s mỗi đầu | Xong |
| 04 | 3 servo đồng bộ (chân 3, 5, 6) | Xong |
| 05 | 1 LED sáng dần / tắt dần (chân ~11) | Xong |
| 06 | 6 LED chớp so le lẻ/chẵn (chân 11, 10, 9, 6, 5, 3) | Xong |
| 07 | Như 06 + hiệu ứng chạy tăng dần trái → phải, tự chuyển hiệu ứng | Xong |
| 08 | 5 servo: đồng bộ + lượn sóng trái → phải, tự chuyển hiệu ứng | Xong |
| 09 | 5 servo quay đồng bộ cùng lúc, mạch mới (chân 11, 9, 6, 5, 3) | Xong |
| 10 | Bài 1.3 thử với 4 LED (13, 12, 11, 10): nút 1 chẵn, nút 2 lẻ, nút 3 đảo (nút 7, 6, 5) | Xong |
| 11 | Bài 1.3 đủ 8 LED (13 → 6), nút 1, 2, 3 ở chân 4, 3, 2 | Xong |
| 12 | Bài 1.4: 3 servo (9, 10, 11), nút 1 → 90/90/180°, nút 2 → 180/180/90°, đổi chiều mỗi 0,5 s (nút 3, 2) | Xong |
| 13 | 2 nút bật/tắt 2 LED: nút 1 (chân 2) → LED đỏ (4), nút 2 (chân 3) → LED xanh (5) | Xong |

Ngày 27/10/2026, cả 8 bản qua `tests/run_all.sh`: chân, g++ giả lập, test logic 05–08, arduino-cli cho Uno.
Flash 3–11 %, RAM tối đa 15 % (bản 02, LCD).

**Ghi chú bản 08:** chân `SERVO_PINS[] = {3, 5, 6, 9, 10}` đọc từ ảnh mạch Tinkercad, chưa được người dùng
xác nhận trên mạch thật. Mặc định 2 vòng đồng bộ + 2 vòng lượn sóng, cả chương trình 23,6 giây.

**Ghi chú bản 09 (31/10/2026):** mạch Tinkercad mới, servo phía trên Arduino, `SERVO_PINS[] = {11, 9, 6, 5, 3}`
đọc từ ảnh, chưa xác nhận trên mạch thật. Một vòng 6,4 giây. Cả 9 bản qua `tests/run_all.sh` ngày 31/10/2026.

**Ghi chú bản 10 (01/11/2026):** bài tập nhóm 1.3 (đề gốc 8 LED, làm thử 4 LED trước). Chân đọc từ ảnh
Tinkercad, chưa xác nhận. Nút có điện trở kéo xuống, `PRESSED = HIGH`; nút 4 (chân 4) chưa dùng. LED trong ảnh chưa
có điện trở 220Ω. Còn bài 1.4 (3 servo, 2 nút) chưa làm. Cả 10 bản qua `tests/run_all.sh` ngày 01/11/2026.

**Ghi chú bản 11 (01/11/2026):** bài 1.3 trên mạch 8 LED, logic giữ nguyên bản 10. `LED_PINS[] = {13, 12, 11, 10, 9, 8, 7, 6}`,
`BUTTON_PINS[] = {4, 3, 2}` đọc từ ảnh, chưa xác nhận; nút kéo xuống, `PRESSED = HIGH`. LED trong ảnh chưa có điện trở.
Bài 1.4 vẫn chưa làm. Cả 11 bản qua `tests/run_all.sh` ngày 01/11/2026.

**Ghi chú bản 12 (02/11/2026):** bài 1.4. Mạch Tinkercad mới, `SERVO_PINS[] = {9, 10, 11}` (trái → phải),
`BUTTON_PINS[] = {3, 2}` (nút dưới, nút trên) đọc từ ảnh, chưa xác nhận. Nút nối chân ↔ GND, không điện trở,
`PRESSED = LOW`. Cách hiểu đề: "quay liên tục cách nhau 0,5s" = quay tới góc rồi về 0°, đổi chiều mỗi 0,5 s; nhấn
một lần là chạy mãi; servo 1, 2 là "2 servo", servo 3 là "1 servo". Chưa hỏi người dùng có cần nút dừng không.
Cả 12 bản qua `tests/run_all.sh` ngày 02/11/2026.

**Ghi chú bản 13 (02/11/2026):** mạch Tinkercad mẫu của kênh M&E Automation, chân đọc từ ảnh, chưa xác nhận. Hai nút
nối khác kiểu: nút 1 kéo xuống (điện trở → GND, chân kia → 5V), nút 2 nối thẳng GND, nên `PRESSED[] = {HIGH, LOW}`
theo từng nút. LED có điện trở. Chọn kiểu nhấn để đổi trạng thái (toggle); người dùng chưa nói có muốn kiểu
giữ nút mới sáng không. Cả 13 bản qua `tests/run_all.sh` ngày 02/11/2026.

**Ý tưởng đã đề xuất, người dùng chưa chốt:** hiệu ứng thứ 3 cho bản 08 (sóng chạy ngược phải → trái),
làm mềm chuyển động servo ở hai đầu (easing). Chưa làm gì cho đến khi người dùng yêu cầu.

## Quy ước code (giữ đúng khi viết prototype mới)

- Thư mục mới: `prototype-NN-ten-khong-dau/sketch.ino`, tên do người dùng đặt (xem mục bắt buộc ở đầu file),
  NN tăng dần (bản tiếp theo là 14).
- Đầu file: một dòng mô tả `– Arduino Uno`, rồi khối `// Mạch:` liệt kê từng dây nối, rồi mô tả hiệu ứng.
- Comment tiếng Việt có dấu; tên hằng/hàm theo kiểu đã có (`SO_LE_`, `CHAY_`, `DONG_BO_`, `SONG_`,
  `levelSoLe()`, `angleAt()`), tên tiếng Việt không dấu khi là tên hiệu ứng.
- **Không dùng `delay()`.** Dùng `millis()`; với hiệu ứng, tính giá trị (độ sáng, góc) là hàm của thời gian
  trong vòng, `loop()` không bao giờ chặn.
- Gom mọi thông số chỉnh được lên đầu file, đơn vị mili giây, có comment ý nghĩa; số vòng mỗi hiệu ứng
  đặt 0 để bỏ hiệu ứng đó.
- Chân dạng mảng `*_PINS[]` theo thứ tự **trái → phải** trên breadboard, số lượng tính bằng `sizeof`.
- Chỉ ghi ra chân/servo khi giá trị thay đổi (`lastPwm[]`, `lastAngle[]`).
- LED mờ dần dùng `gammaPwm()` (bình phương mức sáng) cho mắt thấy đều.
- Mỗi vòng hiệu ứng servo bắt đầu và kết thúc cùng một góc, và servo phải dừng trước khi đổi chiều,
  để chuyển hiệu ứng không bị giật.
- Bản mới tránh `static_assert` và `constexpr` (bản 07 có `static_assert`, chạy được trên Wokwi) vì chưa
  chắc trình biên dịch của Tinkercad hỗ trợ; dùng kiểm tra lúc chạy, ví dụ `if (SHOW_MS == 0) return;`.

## Khi thêm hoặc sửa một prototype

Quy trình đầy đủ nằm trong skill `arduino-prototype`; tóm tắt:

1. Bản mới: hỏi tên dự án trước, rồi viết `sketch.ino` theo quy ước trên.
2. Logic theo thời gian thì viết thêm `tests/test_NN_*.cpp` theo mẫu `test_07_chay.cpp` / `test_08_servo.cpp`:
   chạy đủ 2 chu kỳ, kiểm tra không có cú nhảy, điểm chuyển hiệu ứng, khoảng dừng. Nếu cần thêm hàm Arduino
   thì bổ sung vào `tests/mock/`.
3. Chạy `bash tests/run_all.sh`. Phải ra `TẤT CẢ ĐẠT`.
4. Cập nhật `README.md`: thêm dòng vào bảng đầu, mục **Sơ đồ nối dây**, một gạch đầu dòng ở
   **Logic chính**, và **Xử lý sự cố** nếu có lỗi dễ gặp mới.

## An toàn phần cứng (luôn nhắc khi liên quan)

- Uno có PWM ở **3, 5, 6, 9, 10, 11**. `Servo.h` chiếm Timer1 nên khi có servo, `analogWrite()` trên **9, 10**
  không chạy. Bản 08 đã dùng 3, 5, 6, 9, 10 cho servo, chỉ còn **11** cho LED mờ dần.
- LED thật cần điện trở **220Ω** nối tiếp; không nối LED thẳng 5V–GND.
- Dây đỏ servo vào **5V**, không vào **Vin** (9–12V khi cắm adapter) hay **3.3V** (chỉ ~50 mA, SG90 cần 4,8–6V).
- Nhiều servo cùng quay vượt 500 mA của cổng USB → Arduino tự reset. Dùng nguồn 5V riêng
  (3 servo ≥ 2A, 5 servo ≥ 3A) và luôn **nối chung GND** với Arduino. Tinkercad/Wokwi không mô phỏng giới hạn này.
- Mạch thật của bản 04: người dùng từng cấp 1 servo bằng 3.3V, đã khuyên chuyển sang 5V.
