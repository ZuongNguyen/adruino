---
name: arduino-prototype
description: Quy trình làm hoặc sửa một prototype Arduino Uno trong dự án adruino (Wokwi/Tinkercad) – chốt mạch và chân, kiểm tra an toàn phần cứng, viết sketch theo quy ước trong worktree riêng, cập nhật README, giao ChatGPT kiểm tra, rồi commit và đẩy lên GitHub. Dùng khi người dùng nói "làm bản 09", "thêm hiệu ứng", "đổi chức năng bản NN", gửi ảnh mạch Tinkercad, hoặc hỏi nối dây/chân cho dự án này.
---

# Làm prototype Arduino Uno

Quy ước code và an toàn phần cứng nằm trong `CLAUDE.md` ở thư mục gốc – đọc lại trước khi viết. Trả lời bằng tiếng Việt.

## 0. Hỏi tên dự án (bắt buộc, trước khi tạo file)

- Hỏi người dùng tên dự án và **chờ trả lời**, kể cả khi đề bài và ảnh mạch đã rõ. Không tự đặt tên.
- So tên với các thư mục `prototype-*` đã có. Trùng hoặc gần giống (cùng mạch, cùng bài) thì hỏi: sửa bản cũ hay tạo bản mới.
- Chỉ sửa lỗi một bản đã có thì không tạo gì mới, chuyển sang skill `arduino-sua-loi`. Tối ưu thì dùng `arduino-toi-uu`.
- Có tên rồi thì vào worktree trước khi sửa file: phiên đang ở thư mục chính thì gọi `EnterWorktree` với tên không dấu
  (ví dụ `ban-16-ten-du-an`). Chia việc Claude/ChatGPT xem mục "Bắt buộc: worktree…" trong `CLAUDE.md`.

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

- Thư mục `prototype-NN-ten-khong-dau/sketch.ino`, tên lấy từ bước 0, NN = số lớn nhất hiện có + 1.
- Đầu file: dòng mô tả `– Arduino Uno`, khối `// Mạch:`, mô tả hiệu ứng.
- Không `delay()`; giá trị là hàm của thời gian trong vòng (`levelXxx(i, t)`, `angleAt(...)`).
- Thông số chỉnh được gom lên đầu, đơn vị ms, có comment; số vòng mỗi hiệu ứng đặt 0 để bỏ.
- Không `static_assert`/`constexpr` (Tinkercad); kiểm tra lúc chạy như `if (SHOW_MS == 0) return;`.
- Bắt chước bản gần nhất cùng loại: LED → bản 07, servo → bản 08, nút + LCD → bản 02.

- Thư viện mới → Claude cài `arduino-cli lib install "<tên>"` và thêm `libraries.txt` cho Wokwi; mock trong `tests/mock/` để ChatGPT viết.

## 4. Cập nhật tài liệu

1. `README.md`: dòng mới trong bảng đầu, mục **Sơ đồ nối dây**, gạch đầu dòng ở **Logic chính**, **Xử lý sự cố** nếu có lỗi mới.
2. `CLAUDE.md`: dòng mới trong bảng **Tiến trình**, ghi chú chân chưa xác nhận và ý tưởng người dùng chưa chốt.

## 5. Giao ChatGPT kiểm tra

Claude **không** tự chạy `tests/run_all.sh` hay viết test. Giao cho ChatGPT, chạy nền vì mất vài phút:

```bash
bash scripts/chatgpt-kiem-tra.sh "Thêm bản NN: <làm gì>. Cần test: <...>"
```

Lời giao việc nói rõ ChatGPT cần kiểm tra gì:
- Logic theo thời gian → test `tests/test_NN_ten.cpp` theo mẫu `test_07_chay.cpp` / `test_08_servo.cpp`: chạy 2 chu kỳ từng ms,
  không nhảy giá trị (ngưỡng tính từ thông số), điểm chuyển hiệu ứng, thứ tự trái → phải, khoảng dừng.
- Có nút → nhấn, nhả, giữ nút, nhấn lần hai, dội phím (mẫu `test_13_nut_bat_tat.cpp`).
- Hàm Arduino mới (`Serial`, `tone`, `pulseIn`…) hoặc thư viện mới → cần thêm mock.

Đọc `.chatgpt/bao-cao.md`:
- **CHƯA ĐẠT**: sửa sketch đúng lỗi được báo rồi giao lại. Nghĩ ChatGPT sai (test đặt ngưỡng sai, hiểu sai đề) thì nói với
  người dùng kèm lý do, không tự bỏ qua. 3 vòng chưa đạt thì dừng, hỏi người dùng.
- RAM > 75 % thì báo người dùng.
- Script trả mã 2 (ChatGPT không chạy được) → báo người dùng, không tự chạy test thay.

## 6. Đóng gói và trả lời người dùng

**ĐẠT** thì commit trên nhánh worktree, gộp vào `main`, đẩy lên GitHub theo mục "Bắt buộc: worktree…" trong `CLAUDE.md`
(người dùng đã đồng ý trước, không hỏi lại).

Gửi người dùng: bảng nối dây, cách dán vào Wokwi/Tinkercad, thông số chỉnh nhanh, cảnh báo phần cứng liên quan,
tóm tắt báo cáo ChatGPT (Flash/RAM, điều chưa kiểm tra được), commit đã lên GitHub.
