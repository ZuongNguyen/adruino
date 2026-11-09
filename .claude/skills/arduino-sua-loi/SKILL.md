---
name: arduino-sua-loi
description: Sửa lỗi một sketch Arduino Uno đã có trong dự án adruino, ngay trên file cũ thay vì tạo file mới – phân loại triệu chứng, tái hiện bằng test, sửa tối thiểu, chạy lại kiểm tra. Dùng khi người dùng báo "lỗi", "không chạy", "không biên dịch được", "nút không ăn", "servo giật", "Arduino tự reset", "LED không sáng", "sửa bản NN", hoặc dán thông báo lỗi của Arduino IDE.
---

# Sửa lỗi sketch Arduino Uno

Trả lời bằng tiếng Việt. **Sửa ngay trong file của bản đang lỗi.** Không tạo bản sao, thư mục mới hay số bản mới. Chưa rõ người dùng nói bản nào thì hỏi lại, đừng đoán.

## 1. Phân loại triệu chứng

| Triệu chứng | Nguyên nhân hay gặp | Kiểm tra |
|---|---|---|
| `main file missing`, IDE đòi tạo thư mục mới | Tên `.ino` khác tên thư mục chứa nó | So tên file với tên thư mục |
| `No such file or directory` khi `#include` | Thiếu thư viện | `arduino-cli lib list`, `libraries.txt` |
| `#error "Đây là file test..."` | Dán nhầm file trong `tests/` | Dán đúng file trong `prototype-NN-*/` |
| Nút không ăn hoặc sáng ngược | `PRESSED` sai kiểu nối, chân thả nổi | Nút kéo xuống là `HIGH`, nối GND hoặc `INPUT_PULLUP` là `LOW` |
| Nhấn một lần đổi nhiều lần | Dội phím | Thời gian chống dội, so với bản 01 |
| LED không mờ dần | Chân không PWM, hoặc chân 9, 10 khi có `Servo.h` | Chỉ 3, 5, 6, 9, 10, 11 có PWM |
| LCD trống hoặc toàn ô vuông | Sai địa chỉ I2C, chưa chỉnh tương phản | 0x27 hay 0x3F, A4 = SDA, A5 = SCL |
| Servo rung, Arduino tự reset | Thiếu dòng, chưa nối chung GND | Nguồn 5V riêng, GND chung, dây đỏ vào 5V |
| Sai sau khi chạy lâu, số nhảy lạ | `int` 16 bit tràn, so sánh `millis()` sai | Ép `(long)`, dùng `millis() - moc` |
| Mô phỏng chạy, mạch thật không | Sai dây, thiếu điện trở, thiếu dòng | Đối chiếu khối `// Mạch:` đầu file với mạch thật |

Xem thêm mục **Xử lý sự cố** trong `README.md`. Trong `CLAUDE.md`, nhiều bản có chân đọc từ ảnh và **chưa xác nhận**: nghi sai chân thì hỏi người dùng trước khi sửa code.

## 2. Tái hiện trước khi sửa

- Lỗi logic: thêm trường hợp vào `tests/test_NN_*.cpp` để test **fail** đúng như lỗi, rồi mới sửa.
- Lỗi biên dịch: chạy `bash tests/compile_uno.sh` để thấy đúng thông báo.
- Lỗi phần cứng: không viết test được, nói rõ cần người dùng kiểm tra gì trên mạch.
- Cần xem giá trị trên mạch thật: tạm thêm `Serial.begin(9600)` và `Serial.println(F("..."))`, tránh chân 0, 1, rồi **xóa** sau khi tìm ra lỗi.

## 3. Sửa và kiểm tra

- Chỉ sửa phần gây lỗi, giữ đúng quy ước trong `CLAUDE.md`, không tiện tay viết lại chỗ khác.
- Chạy `bash tests/run_all.sh`, phải ra **TẤT CẢ ĐẠT**.
- Có lỗi dễ gặp mới thì thêm vào **Xử lý sự cố** trong `README.md`.

## 4. Báo kết quả

Nói: nguyên nhân, dòng đã sửa, file nào cần mở lại hoặc dán lại vào Wokwi/Tinkercad, điều gì chưa kiểm tra được trên mạch thật. Nhắc cảnh báo phần cứng nếu liên quan. Đề nghị commit với lời nhắn dạng `Sửa bản NN: <lỗi đã sửa>`, chỉ commit và push khi người dùng đồng ý.
