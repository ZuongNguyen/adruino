---
name: arduino-sua-loi
description: Sửa lỗi một sketch Arduino Uno đã có trong dự án adruino, ngay trên file cũ thay vì tạo file mới – phân loại triệu chứng, sửa tối thiểu trong worktree, giao ChatGPT tái hiện và kiểm tra lại, rồi commit và đẩy lên GitHub. Dùng khi người dùng báo "lỗi", "không chạy", "không biên dịch được", "nút không ăn", "servo giật", "Arduino tự reset", "LED không sáng", "sửa bản NN", hoặc dán thông báo lỗi của Arduino IDE.
---

# Sửa lỗi sketch Arduino Uno

Trả lời bằng tiếng Việt. **Sửa ngay trong file của bản đang lỗi.** Không tạo bản sao, thư mục mới hay số bản mới. Chưa rõ người dùng nói bản nào thì hỏi lại, đừng đoán.

Làm trong worktree riêng (phiên đang ở thư mục chính thì `EnterWorktree`, tên kiểu `sua-ban-NN`). Claude chỉ sửa code;
tái hiện lỗi, viết test và chạy kiểm tra là việc của ChatGPT, xem mục "Bắt buộc: worktree…" trong `CLAUDE.md`.

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

## 2. Tìm nguyên nhân

- Đọc code đối chiếu bảng trên. Nguyên nhân rõ thì sửa luôn (bước 3).
- Chưa rõ, hoặc là lỗi logic khó thấy: giao ChatGPT tái hiện trước, chưa sửa gì:
  `bash scripts/chatgpt-kiem-tra.sh "Tái hiện lỗi <triệu chứng> ở bản NN: viết test fail đúng như lỗi, chưa sửa sketch"`.
  Lỗi biên dịch thì nhờ ChatGPT chép nguyên thông báo của `compile_uno.sh` vào báo cáo.
- Lỗi phần cứng: không viết test được, nói rõ cần người dùng kiểm tra gì trên mạch.
- Cần xem giá trị trên mạch thật: tạm thêm `Serial.begin(9600)` và `Serial.println(F("..."))`, tránh chân 0, 1, rồi **xóa** sau khi tìm ra lỗi.

## 3. Sửa và giao kiểm tra

- Chỉ sửa phần gây lỗi, giữ đúng quy ước trong `CLAUDE.md`, không tiện tay viết lại chỗ khác.
- Có lỗi dễ gặp mới thì thêm vào **Xử lý sự cố** trong `README.md`.
- Giao ChatGPT: `bash scripts/chatgpt-kiem-tra.sh "Sửa bản NN: <lỗi gì, sửa dòng nào>. Viết test chặn lỗi này tái diễn."`.
  CHƯA ĐẠT thì sửa tiếp và giao lại; ĐẠT thì commit, gộp `main`, đẩy lên GitHub theo `CLAUDE.md`, lời nhắn `Sửa bản NN: <lỗi đã sửa>`.

## 4. Báo kết quả

Nói: nguyên nhân, dòng đã sửa, file nào cần mở lại hoặc dán lại vào Wokwi/Tinkercad, tóm tắt báo cáo ChatGPT, điều gì chưa kiểm tra được trên mạch thật, commit đã lên GitHub. Nhắc cảnh báo phần cứng nếu liên quan.
