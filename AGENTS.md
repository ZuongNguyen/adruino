# Hướng dẫn cho ChatGPT (Codex) – dự án adruino

Chuỗi prototype **Arduino Uno** của Dai Duong, mô phỏng bằng Wokwi hoặc Tinkercad. Trả lời bằng **tiếng Việt**.
Quy ước code, an toàn phần cứng và ghi chú từng bản (chân nào chưa xác nhận…) nằm trong `CLAUDE.md`. Đọc file đó trước khi kiểm tra.

## Chia việc

| Ai | Làm gì |
|---|---|
| Claude | Viết và sửa code (`prototype-*/`), cập nhật `README.md`, `CLAUDE.md`, commit và đẩy lên GitHub |
| ChatGPT (bạn) | Kiểm tra lỗi và thử nghiệm: chạy bộ kiểm tra, rà code, viết thêm test, báo cáo |

Claude làm mỗi việc trong một worktree riêng (`.claude/worktrees/<tên>`, nhánh `worktree-<tên>` hoặc `claude/<tên>`) rồi gọi bạn bằng
`bash scripts/chatgpt-kiem-tra.sh`. Bạn đang chạy ngay trong worktree đó.

## Được và không được

- **Được** thêm hoặc sửa file trong `tests/` (test mới `tests/test_NN_*.cpp`, mock trong `tests/mock/`) và ghi vào `build/`.
- **Không** sửa `prototype-*/`, `README.md`, `CLAUDE.md`, `AGENTS.md`, `.claude/`, `.agents/`, `scripts/`.
  Thấy lỗi thì ghi vào báo cáo để Claude sửa.
- **Không** chạy `git add`, `git commit`, `git push`, `git stash`, `git checkout`; không tạo hay đổi nhánh; không cài thư viện;
  không nạp code lên board.
- Chỉ báo những gì đã thật sự chạy. Ở đây không có Tinkercad/Wokwi và không có mạch thật: biên dịch đạt không chứng minh
  mạch đúng, đừng viết như đã chạy mô phỏng.

## Các bước kiểm tra

1. **Phạm vi**: `git status`, `git diff main...HEAD` và phần chưa commit. Lời giao việc của Claude nói thêm đã làm gì, bản nào.
2. **Chạy toàn bộ**: `bash tests/run_all.sh` (kiểm tra chân, biên dịch giả lập g++, test logic, biên dịch thật cho Uno bằng
   arduino-cli). Phải ra `TẤT CẢ ĐẠT`. Ghi lại Flash/RAM của bản đã đổi; RAM trên 75 % là lỗi cần báo.
   Sandbox không đọc được g++ (WinLibs) và `Arduino15` trong hồ sơ người dùng, nên **xin chạy lệnh này ngoài sandbox**
   (escalation) ngay từ đầu. Không thay g++/arduino-cli bằng trình biên dịch khác (MSVC…): kết quả đó không tính là ĐẠT.
3. **Đọc kỹ sketch đã đổi**, đối chiếu `CLAUDE.md`:
   - khối `// Mạch:` đầu file khớp với các mảng `*_PINS[]` và với đề bài;
   - an toàn phần cứng: PWM chỉ ở 3, 5, 6, 9, 10, 11; có `Servo.h` thì 9, 10 mất PWM; A4/A5 để cho I2C; tránh 0, 1;
     dây đỏ servo vào 5V, từ 2 servo trở lên cần nguồn riêng và chung GND; LED có điện trở 220Ω;
   - quy ước: không `delay()`, không `static_assert`/`constexpr`; so thời gian kiểu `millis() - moc >= KHOANG`;
     `int` 16 bit không tràn; chỉ ghi chân/servo khi giá trị đổi; mỗi vòng hiệu ứng servo bắt đầu và kết thúc cùng một góc;
   - nút: `PRESSED` khớp kiểu nối (kéo xuống → `HIGH`, nối GND hoặc `INPUT_PULLUP` → `LOW`), có chống dội,
     giữ nút không làm đổi trạng thái nhiều lần.
   Cần soi dây hoặc logic kỹ hơn thì dùng skill `.agents/skills/arduino-wiring-check` và `.agents/skills/circuit-logic-review`.
4. **Viết test** khi logic theo thời gian hoặc theo nút chưa có test: `tests/test_NN_*.cpp` theo mẫu `tests/test_07_chay.cpp`,
   `tests/test_08_servo.cpp`, `tests/test_13_nut_bat_tat.cpp` (chạy ít nhất 2 chu kỳ từng ms; không có cú nhảy; điểm chuyển hiệu ứng;
   khoảng dừng; ngưỡng tính từ thông số trong sketch, không gõ số cố định). Viết xong chạy lại bước 2.
5. **Nghi có lỗi** thì cố tái hiện bằng một test fail đúng như lỗi, rồi ghi vào báo cáo. Không tự sửa sketch.

## Báo cáo

Tin nhắn cuối cùng của bạn được lưu vào `.chatgpt/bao-cao.md` để Claude đọc. Dòng đầu tiên viết đúng
`KẾT LUẬN: ĐẠT` hoặc `KẾT LUẬN: CHƯA ĐẠT`, không in đậm, không thêm gì khác trên dòng đó. Sau đó theo mẫu:

```
KẾT LUẬN: ĐẠT

## Đã chạy
- bash tests/run_all.sh → TẤT CẢ ĐẠT (hoặc CÓ LỖI: bước nào)
- Bản NN: Flash x %, RAM y %

## Lỗi cần Claude sửa
1. prototype-NN-.../sketch.ino:dòng – lỗi gì – cách tái hiện hoặc test nào fail – gợi ý sửa
(ghi "Không có" nếu không có)

## Góp ý không bắt buộc
- ...

## Test đã thêm hoặc sửa
- tests/... – kiểm tra điều gì (ghi "Không có" nếu không có)

## Chưa kiểm tra được
- ví dụ: chân đọc từ ảnh chưa xác nhận, chưa thử trên mạch thật
```

Chỉ ghi `ĐẠT` khi `run_all.sh` ra `TẤT CẢ ĐẠT` và mục "Lỗi cần Claude sửa" là "Không có".
