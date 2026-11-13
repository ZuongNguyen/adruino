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

## Bắt buộc: worktree, Claude code → ChatGPT kiểm tra → Claude đóng gói

Người dùng chốt ngày 13/11/2026. Ở dự án này **Claude chỉ viết code**. Kiểm tra lỗi và thử nghiệm là việc của
**ChatGPT** (app Codex trên máy, Claude gọi qua `codex exec`). ChatGPT chỉ báo lỗi và viết test, không sửa sketch.
Claude không tự chạy `tests/run_all.sh` hay `/code-review` thay ChatGPT.

1. **Worktree.** Mỗi việc (bản mới, sửa lỗi, tối ưu) làm trong worktree riêng `.claude/worktrees/<tên>`.
   Phiên đang ở thư mục chính thì gọi `EnterWorktree` (tên không dấu, ví dụ `ban-16-...`, `sua-ban-12`) trước khi sửa file.
   `EnterWorktree` tạo nhánh `worktree-<tên>`; worktree do app desktop tạo thì nhánh `claude/<tên>`. Xem tên bằng `git branch --show-current`.
   Worktree tạo từ `main` trên máy (`worktree.baseRef = head` trong `~/.claude/settings.json`).
2. **Claude code**: sketch, `README.md`, `CLAUDE.md` theo mục "Khi thêm hoặc sửa một prototype".
3. **Giao ChatGPT**: `bash scripts/chatgpt-kiem-tra.sh "<đã làm gì, bản nào, cần test gì>"`, chạy nền (vài phút).
   Báo cáo ở `.chatgpt/bao-cao.md` (git bỏ qua), nhật ký ở `.chatgpt/nhat-ky.txt`. Mã thoát 0 = ĐẠT, 1 = CHƯA ĐẠT,
   2 = ChatGPT không chạy được (báo người dùng, không tự chạy test thay). Việc ChatGPT phải làm ghi trong `AGENTS.md`.
4. **CHƯA ĐẠT**: Claude sửa đúng các lỗi ở mục "Lỗi cần Claude sửa" rồi giao lại bước 3. Lỗi ChatGPT báo mà Claude
   thấy không đúng thì nói với người dùng, không tự bỏ qua. Sau 3 vòng vẫn chưa đạt thì dừng và hỏi người dùng.
5. **ĐẠT → đóng gói.** Người dùng đã giao Claude commit và đẩy lên GitHub sau khi ChatGPT báo ĐẠT, không cần hỏi lại:
   - Claude sửa code sau lần ĐẠT cuối thì phải giao ChatGPT lại.
   - Commit trên nhánh worktree, gồm cả test ChatGPT viết. Lời nhắn tiếng Việt kiểu `Thêm bản NN: ...` / `Sửa bản NN: ...`,
     thêm dòng `Kiểm tra: ChatGPT (Codex), tests/run_all.sh ĐẠT`.
   - Gộp vào main từ thư mục chính (dòng đầu của `git worktree list`): `git -C <thư mục chính> merge --ff-only <nhánh worktree>`.
     Không fast-forward được (main đã đi tiếp) thì `git rebase main` trong worktree; rebase có xung đột thì giao ChatGPT lại.
   - `git -C <thư mục chính> push origin main` lên `ZuongNguyen/adruino`.
   - Báo người dùng: đã làm gì, tóm tắt báo cáo ChatGPT (Flash/RAM, điều chưa kiểm tra được), commit nào đã lên GitHub.
     Worktree để lại, người dùng tự dọn hoặc bảo Claude dọn.

## Skill dùng cho dự án

| Việc | Skill / công cụ |
|---|---|
| Làm bản mới, đổi chức năng | `arduino-prototype` |
| Sửa lỗi bản đã có, ngay trên file cũ | `arduino-sua-loi` |
| Tối ưu Flash/RAM, gọn code | `arduino-toi-uu` |
| Kiểm tra lỗi, chạy test, viết test | ChatGPT qua `scripts/chatgpt-kiem-tra.sh` (skill của ChatGPT ở `.agents/skills/`) |
| Rà code lặp phần vừa sửa | `/simplify` |
| Lỗi khó, nhiều bước | `engineering:debug` (tìm nguyên nhân; chạy thử vẫn giao ChatGPT) |

Đã xem và loại ngày 09/11/2026: `ESP32:esp32` (khác chip, chỉ dùng nếu chuyển sang ESP32); các skill công khai
`arduino-skills`, `embedded-c`, `avr`, `microcontroller-firmware`, `embedded-systems` (chung chung hoặc nhắm ARM/RTOS,
không có quy tắc Servo/Timer1, phần lớn không xem được nội dung thật, có skill kèm script chạy trên máy hoặc trượt
kiểm định bảo mật); plugin AI Skills, agentic-council, CodeRabbit (quá rộng so với dự án hoặc cần tài khoản riêng).

## Cấu trúc

```
README.md                      Bảng prototype, sơ đồ nối dây, logic chính, cách chạy, xử lý sự cố
AGENTS.md                      Việc của ChatGPT (Codex): các bước kiểm tra, giới hạn, mẫu báo cáo
.agents/skills/                Skill của ChatGPT: arduino-wiring-check, circuit-logic-review
scripts/chatgpt-kiem-tra.sh    Claude gọi ChatGPT kiểm tra worktree hiện tại, báo cáo ra .chatgpt/ (git bỏ qua)
.claude/worktrees/             Worktree của từng việc (git bỏ qua)
prototype-NN-ten/sketch.ino    Mỗi prototype một thư mục, dán thẳng vào Wokwi/Tinkercad
prototype-02-.../libraries.txt Thư viện cho Wokwi (LiquidCrystal I2C; bản 14 có file tương tự cho PCA9685)
tests/run_all.sh               Chạy toàn bộ 4 bước kiểm tra bên dưới
tests/check_pins.py            Kiểm tra chân: PWM, xung đột Servo/Timer1, trùng chân, A4/A5 khi có I2C
tests/compile_all.sh           Biên dịch thử mọi sketch bằng g++ với Arduino giả lập
tests/test_NN_*.cpp            Chạy thử logic theo từng ms (bản 05–15)
tests/compile_uno.sh           Biên dịch thật cho Uno bằng arduino-cli, in Flash/RAM
tests/mock/                    Arduino.h, Servo.h, Wire.h, LiquidCrystal_I2C.h, Adafruit_PWMServoDriver.h giả lập + globals.cpp
build/                         Kết quả biên dịch (git bỏ qua)
```

Công cụ trên máy (cài 27/10/2026 bằng winget): **arduino-cli 1.5.1** (`C:\Program Files\Arduino CLI`, core
`arduino:avr`, thư viện `Servo`, `LiquidCrystal I2C`, từ 09/11/2026 thêm `Adafruit PWM Servo Driver Library` 3.0.3
và `Adafruit BusIO` 1.17.4) và **g++ 16.2** (WinLibs). Shell mở trước khi cài có thể
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
| 14 | Mạch thử nghiệm 1 servo qua PCA9685 (kênh 0, I2C A4/A5): quét đều + đi từng nấc 45° | Xong |
| 15 | Test servo 11/10: 1 servo chân 3, reset về 0° đứng 1 s rồi quét 0° ↔ 180° mãi | Xong |
| 16 | Reset 5 servo: chân 5, 6, 9, 10, 11 (servo 1 → 5) về 0° lần lượt, cách nhau 0,5 s; rồi servo 2 quay chậm lên 45°, servo 3 lên 60° (15°/s, lần lượt) | Xong |

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

**Ghi chú bản 14 (09/11/2026):** tên thư mục do người dùng đặt ("mạch thử nghiệm 1 servo pca 9685"). Người dùng xác nhận
"chân A0" trong đề là **kênh 0 của PCA9685**, không phải chân A0 Arduino. Dây đọc từ ảnh sơ đồ, chưa xác nhận trên mạch thật.
Kiểu quay do mình chọn (đề để tùy ý): 2 vòng quét + 1 vòng đi từng nấc 45°, cả chương trình 20,8 giây. Dùng thư viện Adafruit;
`begin()` có `delay()` bên trong nhưng chỉ chạy lúc khởi động. Tinkercad không có PCA9685 nên bản này chạy trên mạch thật, chưa
thử trên phần cứng. Flash 20 %, RAM 12 %. Cả 14 bản qua `tests/run_all.sh` ngày 09/11/2026.

**Ghi chú bản 15 (10/11/2026):** tên do người dùng đặt ("test servo 11/10", dấu `/` đổi thành `-` trong tên thư mục).
Gần giống bản 03 nhưng người dùng chọn tạo bản mới, giữ bản 03 cho mạch chân 6. Mạch Tinkercad: tín hiệu chân 3 nối thẳng,
nguồn servo qua hàng +/− dưới breadboard từ 5V/GND; đọc từ ảnh, người dùng đã nhờ kiểm tra và mạch đúng, chưa thử trên mạch thật.
"Reset" hiểu là lúc khởi động (cấp điện hoặc nhấn nút RESET trên board): ghi 0° rồi đứng `RESET_MS` = 1 s, sau đó quét như bản 09
mãi (người dùng chọn). Mạch không có nút nên chưa có nút reset riêng. Flash 6 %, RAM 2 %. Cả 15 bản qua `tests/run_all.sh` ngày 10/11/2026.
Ngày 13/11/2026 (lần chạy thử đầu của quy trình ChatGPT): ChatGPT phát hiện khi `millis()` tràn (≈ 49,7 ngày) bản 15 quay lại
giai đoạn reset, servo giật 180° → 0°; ChatGPT viết `tests/test_15_rollover.cpp`. Claude sửa: cờ `dangQuet` + mốc `mocVong` dời bằng
phép trừ unsigned, `angleAt(t)` nhận thời gian trong vòng. Các bản 06, 07 (LED) và 08, 09, 14 (servo) tính pha bằng
`millis() % chu kỳ` nên cũng lệch pha một lần khi tràn (servo giật một cái); chưa sửa, chờ người dùng quyết.

**Ghi chú bản 16 (13/11/2026):** tên do người dùng đặt ("reset 5 servo"). Mạch Tinkercad 5 servo mới, servo xếp dọc bên phải
Arduino. Quy ước của người dùng: servo 1 → 5 theo chân tín hiệu 5, 6, 9, 10, 11, nên `SERVO_PINS[] = {5, 6, 9, 10, 11}` và servo 1 là
servo dưới cùng trong ảnh. Dùng quy ước này cho các đề sau trên mạch này. Dây đọc từ ảnh, chưa thử trên mạch thật.
Người dùng chọn kiểu về lần lượt 1 → 5 (`RESET_GAP_MS` = 500 ms, đặt 0 để về cùng lúc). Cách làm: `write(0)` trước `attach()`
cho xung đầu đã là 0° (không qua 90° mặc định), servo chưa tới lượt chưa `attach()` nên vẫn có điện nhưng chưa có xung điều khiển, chưa kéo dòng để quay.
Bản reset đầu tiên (chỉ về 0°): Flash 5 %, RAM 3 %.
ChatGPT sửa mock `Servo.h` (nhớ góc ghi trước `attach()`, ghi lại lần attach), viết `tests/test_16_reset_5_servo.cpp` và
`tests/test_mock_servo.cpp`, `run_all.sh` chạy thêm biến thể `RESET_GAP_MS = 0` trong `build/`. Cả 16 bản qua `tests/run_all.sh` ngày 13/11/2026.
Cùng ngày người dùng yêu cầu thêm "cho servo 2, 3 quay lần lượt 1 góc 45 độ, set tốc độ servo chậm lại" và chọn **sửa thẳng bản 16**
(không tạo bản mới), quay một lần rồi đứng yên, 45° trong 3 giây. Sau reset chờ `RESET_HOLD_MS` = 0,5 s, servo 2 quay 2,5–5,5 s, nghỉ
`MOVE_PAUSE_MS`, servo 3 quay 6–9 s; `MOVE_SERVOS[]` ghi theo số servo 1 → 5. Bước reset vẫn quay hết tốc độ vì chưa biết góc ban đầu.
Cờ `xong` cho `loop()` thôi tính toán khi servo cuối quay xong. Bản này ChatGPT báo ĐẠT (vòng 2), Flash 7 %, RAM 4 %, commit 3cd7c6d.
Ngay sau đó người dùng yêu cầu "cho servo 3 từ 45 lên 60 độ": hiểu là đổi góc đích của servo 3 thành 60°, servo 2 giữ 45°. Giữ
**tốc độ** người dùng đã chọn (45° trong 3 s = 15°/s), nên servo 3 quay 60° mất 4 s, từ 6 đến 10 s. Thay `MOVE_SERVOS[]`/`MOVE_ANGLE`/`MOVE_MS`
bằng bảng `MOVES[] = {{2, 45}, {3, 60}}` (struct `BuocQuay`) và `MOVE_SPEED_DEG_S` = 15; lịch quay lập trong `setup()`.
ChatGPT báo ĐẠT, cập nhật `test_16_reset_5_servo.cpp` và biến thể `MOVE_SPEED_DEG_S = 0` trong `run_all.sh`. Flash 7 %, RAM 5 %.

**Ý tưởng đã đề xuất, người dùng chưa chốt:** hiệu ứng thứ 3 cho bản 08 (sóng chạy ngược phải → trái),
làm mềm chuyển động servo ở hai đầu (easing). Chưa làm gì cho đến khi người dùng yêu cầu.

## Quy ước code (giữ đúng khi viết prototype mới)

- Thư mục mới: `prototype-NN-ten-khong-dau/sketch.ino`, tên do người dùng đặt (xem mục bắt buộc ở đầu file),
  NN tăng dần (bản tiếp theo là 17).
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

1. Bản mới: hỏi tên dự án trước, vào worktree, rồi viết `sketch.ino` theo quy ước trên.
2. Cập nhật `README.md`: thêm dòng vào bảng đầu, mục **Sơ đồ nối dây**, một gạch đầu dòng ở
   **Logic chính**, và **Xử lý sự cố** nếu có lỗi dễ gặp mới. Thêm dòng **Tiến trình** và ghi chú bản mới vào `CLAUDE.md`.
3. Giao ChatGPT kiểm tra (`scripts/chatgpt-kiem-tra.sh`). Lời giao việc nói rõ cần test gì: logic theo thời gian thì
   test `tests/test_NN_*.cpp` theo mẫu `test_07_chay.cpp` / `test_08_servo.cpp` (2 chu kỳ, không cú nhảy, điểm chuyển hiệu ứng,
   khoảng dừng); hàm Arduino hay thư viện mới thì cần mock trong `tests/mock/`. ChatGPT viết test, Claude không viết.
4. ĐẠT thì đóng gói theo mục "Bắt buộc: worktree…" ở đầu file; ghi Flash/RAM từ báo cáo vào ghi chú bản đó.

## An toàn phần cứng (luôn nhắc khi liên quan)

- Uno có PWM ở **3, 5, 6, 9, 10, 11**. `Servo.h` chiếm Timer1 nên khi có servo, `analogWrite()` trên **9, 10**
  không chạy. Bản 08 đã dùng 3, 5, 6, 9, 10 cho servo, chỉ còn **11** cho LED mờ dần.
- LED thật cần điện trở **220Ω** nối tiếp; không nối LED thẳng 5V–GND.
- Dây đỏ servo vào **5V**, không vào **Vin** (9–12V khi cắm adapter) hay **3.3V** (chỉ ~50 mA, SG90 cần 4,8–6V).
- Nhiều servo cùng quay vượt 500 mA của cổng USB → Arduino tự reset. Dùng nguồn 5V riêng
  (3 servo ≥ 2A, 5 servo ≥ 3A) và luôn **nối chung GND** với Arduino. Tinkercad/Wokwi không mô phỏng giới hạn này.
- Mạch thật của bản 04: người dùng từng cấp 1 servo bằng 3.3V, đã khuyên chuyển sang 5V.
