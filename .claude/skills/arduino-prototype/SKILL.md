---
name: arduino-prototype
description: Quy trình làm hoặc sửa một prototype Arduino Uno trong dự án adruino (Wokwi/Tinkercad) – chốt mạch và chân, kiểm tra an toàn phần cứng, viết sketch theo quy ước, test giả lập, biên dịch thật cho Uno, cập nhật README. Dùng khi người dùng nói "làm bản 09", "thêm hiệu ứng", "sửa prototype NN", gửi ảnh mạch Tinkercad, hoặc hỏi nối dây/chân cho dự án này.
---

# Làm prototype Arduino Uno

Quy ước code và an toàn phần cứng nằm trong `CLAUDE.md` ở thư mục gốc – đọc lại trước khi viết. Trả lời bằng tiếng Việt.

## 1. Chốt mạch trước khi viết code

- Có ảnh mạch: đọc từng dây, lập bảng `linh kiện → chân Arduino` theo thứ tự **trái → phải** trên breadboard.
  Chân nào đọc không chắc (dây che nhau, màu khó phân biệt) thì ghi rõ "chưa chắc" và hỏi lại người dùng.
- Không có ảnh: đề xuất chân, ưu tiên không trùng các bản trước để người dùng ghép mạch dễ.
- Bản mới dựa trên bản cũ: nói rõ giữ nguyên mạch nào, thêm dây nào.

## 2. Kiểm tra an toàn phần cứng (trước khi viết, nhắc người dùng nếu vướng)

| Kiểm tra | Quy tắc |
|---|---|
| Mờ dần / `analogWrite()` | Chỉ chân **3, 5, 6, 9, 10, 11** |
| Có `Servo.h` | **9, 10** mất PWM (Timer1) → LED mờ dần dùng 3, 5, 6, 11 còn trống |
| LCD I2C | **A4 = SDA, A5 = SCL**, không dùng cho việc khác |
| Serial | Tránh chân **0, 1** nếu cần `Serial.print` hoặc nạp code qua USB |
| Servo | Dây đỏ vào **5V** (không Vin, không 3.3V); từ 2 servo trở lên → nguồn 5V riêng (≈ 0,6–1 A mỗi SG90), **nối chung GND** |
| LED | Điện trở **220Ω** nối tiếp mỗi LED |
| Nút bấm | `INPUT_PULLUP`, nút nối chân ↔ GND, không cần điện trở ngoài |
| RAM | Uno chỉ 2 KB: chuỗi cố định dùng `F("...")` khi in, tránh mảng lớn |
| `int` | 16 bit (−32768..32767): nhân thời gian/góc lớn thì ép `(long)` hoặc dùng `unsigned long` |

Tinkercad/Wokwi **không** mô phỏng giới hạn dòng, nên chạy được trên mô phỏng chưa chắc chạy được trên mạch thật – luôn nhắc khi có servo/động cơ.

## 3. Viết sketch

- Thư mục `prototype-NN-mo-ta-khong-dau/sketch.ino`, NN = số lớn nhất hiện có + 1.
- Đầu file: dòng mô tả `– Arduino Uno`, khối `// Mạch:`, mô tả hiệu ứng.
- Không `delay()`; giá trị là hàm của thời gian trong vòng (`levelXxx(i, t)`, `angleAt(...)`).
- Thông số chỉnh được gom lên đầu, đơn vị ms, có comment; số vòng mỗi hiệu ứng đặt 0 để bỏ.
- Không `static_assert`/`constexpr` (Tinkercad); kiểm tra lúc chạy như `if (SHOW_MS == 0) return;`.
- Bắt chước bản gần nhất cùng loại: LED → bản 07, servo → bản 08, nút + LCD → bản 02.

## 4. Kiểm tra

```bash
bash tests/run_all.sh
```

Chạy 4 bước: `check_pins.py` (chân), `compile_all.sh` (g++ giả lập), mọi `tests/test_*.cpp`, `compile_uno.sh` (arduino-cli cho ATmega328P, in Flash/RAM). Phải ra **TẤT CẢ ĐẠT**.

- Logic theo thời gian → viết `tests/test_NN_ten.cpp` theo mẫu `test_07_chay.cpp` / `test_08_servo.cpp`:
  chạy 2 chu kỳ từng ms, kiểm tra không nhảy giá trị (ngưỡng tính từ thông số, không gõ số cố định),
  điểm chuyển hiệu ứng, thứ tự trái → phải, khoảng dừng.
- Cần hàm Arduino mới (`Serial`, `tone`, `pulseIn`…) → bổ sung vào `tests/mock/Arduino.h`.
- Thư viện mới → `arduino-cli lib install "<tên>"`, thêm `libraries.txt` cho Wokwi và mock trong `tests/mock/`.
- RAM > 75% thì báo người dùng.
- Test fail: sửa sketch nếu là lỗi thật; chỉ nới ngưỡng test khi giải thích được vì sao giá trị đó vẫn đúng.

## 5. Cập nhật tài liệu

1. `README.md`: dòng mới trong bảng đầu, mục **Sơ đồ nối dây**, gạch đầu dòng ở **Logic chính**, **Xử lý sự cố** nếu có lỗi mới.
2. `CLAUDE.md`: dòng mới trong bảng **Tiến trình**, ghi chú chân chưa xác nhận và ý tưởng người dùng chưa chốt.
3. Đề nghị commit git (chỉ commit khi người dùng đồng ý).

## 6. Trả lời người dùng

Gửi kèm: bảng nối dây, cách dán vào Wokwi/Tinkercad, thông số chỉnh nhanh, cảnh báo phần cứng liên quan, kết quả test (Flash/RAM).
