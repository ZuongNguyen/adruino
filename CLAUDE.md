# Dự án adruino – Arduino Uno prototypes

Chuỗi prototype lập trình mạch **Arduino Uno** của Dai Duong, mô phỏng bằng **Wokwi** hoặc **Tinkercad**
(người dùng thường gửi ảnh mạch Tinkercad). Dự án chuyển từ Project "adruino" trên claude.ai sang đây
ngày 27/10/2026; bản gốc trên claude.ai vẫn còn.

Trả lời bằng **tiếng Việt**. Tài liệu chính cho người dùng là `README.md`.
Làm hoặc sửa prototype thì dùng skill **`arduino-prototype`** (`.claude/skills/arduino-prototype/SKILL.md`).

## Cấu trúc

```
README.md                      Bảng prototype, sơ đồ nối dây, logic chính, cách chạy, xử lý sự cố
prototype-NN-ten/sketch.ino    Mỗi prototype một thư mục, dán thẳng vào Wokwi/Tinkercad
prototype-02-.../libraries.txt Thư viện cho Wokwi (LiquidCrystal I2C)
tests/run_all.sh               Chạy toàn bộ 4 bước kiểm tra bên dưới
tests/check_pins.py            Kiểm tra chân: PWM, xung đột Servo/Timer1, trùng chân, A4/A5 khi có I2C
tests/compile_all.sh           Biên dịch thử mọi sketch bằng g++ với Arduino giả lập
tests/test_NN_*.cpp            Chạy thử logic theo từng ms (bản 05, 06, 07, 08)
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

Ngày 27/10/2026, cả 8 bản qua `tests/run_all.sh`: chân, g++ giả lập, test logic 05–08, arduino-cli cho Uno.
Flash 3–11 %, RAM tối đa 15 % (bản 02, LCD).

**Ghi chú bản 08:** chân `SERVO_PINS[] = {3, 5, 6, 9, 10}` đọc từ ảnh mạch Tinkercad, chưa được người dùng
xác nhận trên mạch thật. Mặc định 2 vòng đồng bộ + 2 vòng lượn sóng, cả chương trình 23,6 giây.

**Ý tưởng đã đề xuất, người dùng chưa chốt:** hiệu ứng thứ 3 cho bản 08 (sóng chạy ngược phải → trái),
làm mềm chuyển động servo ở hai đầu (easing). Chưa làm gì cho đến khi người dùng yêu cầu.

## Quy ước code (giữ đúng khi viết prototype mới)

- Thư mục mới: `prototype-NN-mo-ta-khong-dau/sketch.ino`, NN tăng dần (bản tiếp theo là 09).
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

1. Viết `sketch.ino` theo quy ước trên.
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
