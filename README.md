# Dự án Arduino – Dai Duong

Các prototype chạy trên **Arduino Uno**, mô phỏng bằng Wokwi hoặc Tinkercad.

| Prototype | Nội dung | File |
|---|---|---|
| 01 – LED nút bấm | Nhấn nút để bật/tắt LED, có chống dội phím | `prototype-01-led-nut-bam/sketch.ino` |
| 02 – LED nút bấm + LCD | Như bản 01, thêm LCD 16x2 I2C hiện " Dai Duong " và trạng thái LED | `prototype-02-led-nut-bam-lcd/sketch.ino`, `libraries.txt` |
| 03 – Servo 180° | Servo quay qua lại 0° ↔ 180°, dừng 0,5 giây ở mỗi đầu | `prototype-03-servo-180/sketch.ino` |
| 04 – 3 servo đồng bộ | 3 servo cùng quay qua lại 0° ↔ 180° với cùng một góc | `prototype-04-3-servo-dong-bo/sketch.ino` |
| 05 – LED sáng dần, tắt dần | LED sáng dần trong ~1,5 giây rồi tắt dần, lặp lại | `prototype-05-led-sang-dan-tat-dan/sketch.ino` |
| 06 – 6 LED chớp so le | Nhóm LED lẻ (1, 3, 5) và chẵn (2, 4, 6) thay phiên chớp sáng rồi tắt dần | `prototype-06-6-led-chop-so-le/sketch.ino` |
| 07 – 6 LED so le + chạy tăng dần | Như bản 06, thêm hiệu ứng LED lần lượt sáng dần từ trái qua phải đến khi sáng hết dãy; tự chuyển qua lại giữa 2 hiệu ứng | `prototype-07-6-led-so-le-chay-tang-dan/sketch.ino` |
| 08 – 5 servo đồng bộ + lượn sóng | 5 servo cùng quay 0° ↔ 180°, rồi chuyển sang lượn sóng: lần lượt quay lên từ trái qua phải, lần lượt quay về; tự chuyển qua lại giữa 2 hiệu ứng | `prototype-08-5-servo-dong-bo-luon-song/sketch.ino` |

## Sơ đồ nối dây

### Prototype 01

| Linh kiện | Chân linh kiện | Nối tới Arduino |
|---|---|---|
| LED đỏ | Chân dương (A) | Điện trở 220Ω → chân **8** |
| LED đỏ | Chân âm (K) | **GND** (hàng chân digital) |
| Nút bấm | Một chân | Chân **2** |
| Nút bấm | Chân đối diện | **GND** (hàng chân nguồn) |

### Prototype 02

Giữ nguyên toàn bộ dây của bản 01, thêm:

| Linh kiện | Chân linh kiện | Nối tới Arduino |
|---|---|---|
| LCD 16x2 I2C | GND | **GND** |
| LCD 16x2 I2C | VCC | **5V** |
| LCD 16x2 I2C | SDA | **A4** |
| LCD 16x2 I2C | SCL | **A5** |

### Prototype 03

| Linh kiện | Dây servo | Nối tới Arduino |
|---|---|---|
| Servo | Cam (tín hiệu) | Chân **6** |
| Servo | Đỏ (nguồn) | **5V** |
| Servo | Nâu (GND) | **GND** |

### Prototype 04

| Servo | Dây cam (tín hiệu) | Dây đỏ (nguồn) | Dây nâu |
|---|---|---|---|
| 1 – tay tím | Chân **3** | **5V** | **GND** |
| 2 – tay trắng | Chân **5** | **5V** | **GND** |
| 3 – tay cam | Chân **6** | **5V** | **GND** |

> **Mạch thật:** 2 servo cấp 5V, 1 servo cấp 3.3V. Bản vẽ Wokwi chỉ là mô phỏng nên dây nguồn vẽ khác.
>
> ⚠️ **Servo cấp 3.3V:** SG90 thiết kế cho 4,8–6V, nên ở 3.3V servo này yếu và chậm hơn hai servo kia, có thể không theo kịp góc khi có tải. Nếu lấy từ chân 3.3V của Uno (chỉ khoảng 50 mA), servo dễ giật và mạch ổn áp 3.3V có thể bị quá tải. Nên chuyển servo này sang 5V.
>
> **Nguồn ngoài:** luôn nối **chung GND** giữa nguồn và Arduino, nếu không tín hiệu điều khiển mất mốc và servo quay lung tung. 3 servo quay cùng lúc nên dùng nguồn 5V ≥ 2A, tránh kéo quá 500 mA của cổng USB làm Arduino tự reset.
>
> Không cắm dây đỏ servo vào **Vin**: khi cắm adapter 9–12V vào jack DC, Vin ra 9–12V và có thể làm cháy servo.

### Prototype 05

| Linh kiện | Chân linh kiện | Nối tới Arduino |
|---|---|---|
| LED đỏ | Chân dương (chân cong) | Điện trở 220Ω → chân **~11** |
| LED đỏ | Chân âm (chân thẳng) | **GND** |

> ⚠️ Không nối LED thẳng vào **5V** và **GND**: LED khi đó luôn sáng, code không điều khiển được, và thiếu điện trở thì LED thật sẽ cháy. Chân **~11** được chọn vì có PWM và không trùng chân nào ở các prototype trước, kể cả khi ghép với servo.

### Prototype 06

6 LED cắm trên breadboard, xếp từ trái sang phải. Chân dương (chân cong) mỗi LED nối một chân PWM, chân âm nối hàng **GND** trên breadboard (hàng này nối GND của Arduino bằng dây đen).

| LED | Nhóm | Chân Arduino |
|---|---|---|
| 1 | Lẻ | **~11** |
| 2 | Chẵn | **~10** |
| 3 | Lẻ | **~9** |
| 4 | Chẵn | **~6** |
| 5 | Lẻ | **~5** |
| 6 | Chẵn | **~3** |

> Bản này dùng hết 6 chân PWM của Uno. Mạch thật cần thêm điện trở 220Ω nối tiếp mỗi LED. Nếu sau này ghép thêm servo, LED ở chân 9 và 10 sẽ chỉ bật/tắt chứ không mờ dần được.

### Prototype 07

Giữ nguyên mạch prototype 06. Thứ tự trong mảng `LED_PINS[] = {11, 10, 9, 6, 5, 3}` chính là thứ tự từ trái qua phải; nếu đèn chạy sai thứ tự thì đổi lại mảng này cho khớp với vị trí LED thật.

### Prototype 08

5 servo xếp từ trái sang phải dưới breadboard (bản vẽ Tinkercad). Dây đỏ **5V** và dây đen **GND** của Arduino nối xuống hai hàng nguồn dưới cùng của breadboard; dây đỏ và dây nâu/đen của từng servo cắm vào hai hàng đó.

| Servo | Dây tín hiệu | Chân Arduino |
|---|---|---|
| 1 (trái cùng) | Cam | **3** |
| 2 | Vàng | **5** |
| 3 | Xanh nhạt | **6** |
| 4 | Xanh dương | **9** |
| 5 (phải cùng) | Hồng | **10** |

> Chân đọc từ ảnh mạch, nếu servo chạy sai thứ tự hoặc có servo đứng yên thì sửa mảng `SERVO_PINS[] = {3, 5, 6, 9, 10}` cho khớp với dây thật (trái qua phải).
>
> ⚠️ **Mạch thật:** 5 servo SG90 quay cùng lúc kéo dòng vượt xa 500 mA của cổng USB, Arduino sẽ tự reset hoặc servo giật. Cấp cho hàng nguồn breadboard một nguồn 5V ≥ 3A riêng, nối **chung GND** với Arduino, và bỏ dây 5V từ Arduino xuống hàng nguồn. Tinkercad không mô phỏng giới hạn này nên vẫn chạy bình thường.

## Logic chính

- Nút dùng `INPUT_PULLUP`: nhả đọc `HIGH`, nhấn đọc `LOW`, không cần điện trở ngoài.
- Chống dội phím 50 ms bằng `millis()`, không dùng `delay()`.
- LED chỉ đảo trạng thái tại lúc nhấn xuống (HIGH → LOW).
- Bản 02: dòng 1 LCD hiện " Dai Duong " căn giữa; dòng 2 hiện `LED: BAT` / `LED: TAT`, chỉ ghi lại khi trạng thái đổi.
- Bản 03: servo tăng/giảm 1° mỗi 15 ms (≈2,7 giây cho 180°), đổi hướng ở 0° và 180°; dùng `millis()` nên không chặn `loop()`.
- Bản 04: cùng logic bản 03 nhưng dùng mảng `SERVO_PINS[]` + `servos[]`; hàm `writeAll()` gửi cùng một góc cho cả 3 servo trong một lượt nên chúng quay đồng bộ. Thêm servo chỉ cần thêm số chân vào mảng (Uno chạy tối đa 12 servo).
- Bản 05: chạy 4 pha `SANG_DAN` → `GIU_SANG` → `TAT_DAN` → `GIU_TAT` rồi lặp lại. Độ sáng (`analogWrite()` 0–255) tính theo thời gian đã trôi qua trong pha, nên chỉnh tốc độ chỉ cần đổi `FADE_IN_MS` / `FADE_OUT_MS`. Hàm `gammaPwm()` bình phương mức sáng để mắt thấy LED sáng/tắt đều, không bị vọt sáng rồi đứng yên.
- Bản 06: hàm `envelope(t)` tính độ sáng theo thời gian trong lượt: sáng lên trong `ATTACK_MS` (0 = chớp sáng ngay), tắt dần trong `DECAY_MS`, nghỉ `GAP_MS`. Nhóm chẵn chạy trễ đúng nửa vòng so với nhóm lẻ nên hai nhóm sáng tắt so le. Mặc định một vòng là 1,6 giây; chỉ ghi `analogWrite()` khi độ sáng thay đổi.
- Bản 07: chương trình chạy so le `SO_LE_ROUNDS` vòng (mặc định 3) rồi chạy tăng dần `CHAY_ROUNDS` vòng (mặc định 2), lặp lại; cả chương trình dài 10,4 giây. Chạy tăng dần: LED thứ i bắt đầu sáng dần sau `i × CHAY_STEP_MS`, cả dãy sáng hết sau 1,5 giây, giữ 0,5 giây rồi cùng tắt dần trong 0,6 giây. Mỗi hiệu ứng là một hàm tính độ sáng theo thời gian (`levelSoLe()`, `levelChay()`), nên thêm hiệu ứng mới chỉ cần viết thêm một hàm.
- Bản 08: ghép cách tổ chức của bản 04 (mảng servo) và bản 07 (tính theo thời gian, tự chuyển hiệu ứng). Mỗi vòng có 4 đoạn: lượt lên 0° → 180°, dừng, lượt về 180° → 0°, dừng. Hàm `angleAt()` cho servo thứ i bắt đầu trễ `i × stepMs`: đồng bộ là `stepMs = 0` (quay hết 180° trong 2,7 giây như bản 04, dừng 0,5 giây), lượn sóng là `stepMs = 300` (mỗi servo quay hết 180° trong 1 giây, servo cuối tới nơi sau 2,2 giây). Chương trình chạy đồng bộ `DONG_BO_ROUNDS` vòng rồi lượn sóng `SONG_ROUNDS` vòng (mặc định 2 + 2, dài 23,6 giây). Vòng nào cũng bắt đầu và kết thúc ở 0° nên chuyển hiệu ứng không bị giật, và servo luôn dừng ít nhất 0,5 giây trước khi đổi chiều.

## Cách chạy

**Wokwi:** dán `sketch.ino` vào tab sketch. Với bản 02, tạo thêm tab `libraries.txt` và dán nội dung file đó để Wokwi tự cài thư viện LCD.

**Tinkercad:** bấm **Code** (Mã), chuyển sang chế độ **Text** (Văn bản), xóa code cũ rồi dán `sketch.ino`. Thư viện `Servo.h` có sẵn, không cần cài.

**Arduino IDE:** với bản 02, cài thư viện **LiquidCrystal I2C** (Frank de Brabander) trong Library Manager.

**Kiểm tra trước khi dán** (cần `g++`, `python`, `arduino-cli`):

```bash
bash tests/run_all.sh
```

Lệnh này kiểm tra chân (PWM, xung đột servo với chân 9 và 10, trùng chân), biên dịch thử, chạy test hiệu ứng theo từng ms, rồi biên dịch thật cho Uno và in dung lượng Flash/RAM. Kết quả đúng là dòng `TẤT CẢ ĐẠT`.

## Xử lý sự cố

- LCD sáng nhưng không có chữ: đổi địa chỉ `0x27` thành `0x3F`, hoặc vặn biến trở tương phản ở mặt sau module.
- Một lần nhấn mà LED đổi nhiều lần: tăng `DEBOUNCE_MS` lên 80–100.
- LED chỉ bật/tắt chứ không mờ dần: chân LED không có dấu ~ (không có PWM), chuyển sang 3, 5, 6, 9, 10 hoặc 11.
- Một LED không bao giờ sáng: LED bị cắm ngược, xoay lại để chân dương (chân cong/dài) nối chân Arduino, chân âm nối GND.
- Servo thật không quay đủ 180°: thử `servo.attach(SERVO_PIN, 500, 2500);`. Nếu servo kêu rè ở hai đầu thì giảm lại, vì đã chạm giới hạn cơ khí.
- Servo giật hoặc Arduino tự reset: servo lớn (MG996R…) hoặc nhiều servo cùng lúc cần nguồn 5V riêng, nối chung GND với Arduino.
- Thư viện `Servo.h` dùng Timer1 nên `analogWrite()` trên chân 9 và 10 sẽ không hoạt động khi có servo; nếu cần PWM cho LED thì dùng các chân PWM khác (3, 5, 6, 11) chưa bị servo chiếm. Bản 08 đã dùng 3, 5, 6 cho servo nên chỉ còn chân **11** cho LED mờ dần.
- Lượn sóng chạy sai thứ tự (không đi từ trái qua phải): đổi lại thứ tự trong `SERVO_PINS[]` cho khớp vị trí servo thật.
