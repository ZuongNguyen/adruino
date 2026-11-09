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
| 09 – 5 servo đồng bộ | Mạch Tinkercad mới (servo phía trên Arduino): cả 5 servo cùng quay 0° ↔ 180°, dừng 0,5 giây mỗi đầu | `prototype-09-5-servo-dong-bo/sketch.ino` |
| 10 – 4 LED chẵn/lẻ, 3 nút | Bài 1.3 (thử với 4 LED): nút 1 bật LED chẵn, nút 2 bật LED lẻ, nút 3 đảo chẵn ↔ lẻ | `prototype-10-4-led-chan-le-3-nut/sketch.ino` |
| 11 – 8 LED chẵn/lẻ, 3 nút | Bài 1.3 đủ 8 LED: nút 1 bật LED chẵn, nút 2 bật LED lẻ, nút 3 đảo chẵn ↔ lẻ | `prototype-11-8-led-chan-le-3-nut/sketch.ino` |
| 12 – 3 servo, 2 nút | Bài 1.4: nút 1 cho 2 servo quay 90°, 1 servo quay 180°; nút 2 cho 2 servo quay 180°, 1 servo quay 90°; quay qua lại liên tục, đổi chiều mỗi 0,5 giây | `prototype-12-3-servo-2-nut/sketch.ino` |
| 13 – 2 nút bật/tắt 2 LED | Nhấn nút 1 bật/tắt LED đỏ, nhấn nút 2 bật/tắt LED xanh; hai nút nối khác kiểu (kéo xuống và nối thẳng GND) | `prototype-13-2-nut-bat-tat-2-led/sketch.ino` |
| 14 – Mạch thử nghiệm 1 servo PCA9685 | 1 servo ở kênh 0 của mạch PCA9685 (I2C): quét đều 0° ↔ 180°, rồi đi từng nấc 45°; tự chuyển qua lại giữa 2 hiệu ứng | `prototype-14-mach-thu-nghiem-1-servo-pca-9685/sketch.ino`, `libraries.txt` |

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

### Prototype 09

Mạch mới, khác bản 08: 5 servo xếp từ trái sang phải phía trên Arduino, breadboard nằm dưới. Dây đỏ từ chân **5V** và dây nâu từ **GND** của Arduino nối sang hai hàng nguồn trên cùng của breadboard (+ và −); dây đỏ và dây nâu của từng servo cắm vào hai hàng đó.

| Servo | Dây tín hiệu | Chân Arduino |
|---|---|---|
| 1 (trái cùng, cao nhất) | Xanh | **11** |
| 2 | Xanh | **9** |
| 3 | Xanh | **6** |
| 4 | Xanh | **5** |
| 5 (phải cùng) | Xanh | **3** |

> Chân đọc từ ảnh mạch Tinkercad. Vì cả 5 servo nhận cùng một góc nên cắm lẫn thứ tự vẫn chạy đúng; chỉ cần mỗi servo nối một chân có trong `SERVO_PINS[] = {11, 9, 6, 5, 3}`.
>
> ⚠️ **Mạch thật:** giống bản 08, 5 servo quay cùng lúc cần nguồn 5V ≥ 3A riêng, nối **chung GND** với Arduino.

### Prototype 10

Mạch Tinkercad bài 1.3: 4 LED bên trái breadboard, 4 nút bên phải. Dây đỏ từ **5V** và dây đen từ **GND** của Arduino nối vào hai hàng nguồn dưới; hai dây bên trái nối hàng nguồn dưới lên hàng nguồn trên.

| LED (trái → phải) | Chân Arduino | Nhóm |
|---|---|---|
| 1 đỏ | **13** | lẻ |
| 2 xanh dương | **12** | chẵn |
| 3 xanh lá | **11** | lẻ |
| 4 vàng | **10** | chẵn |

Chân âm mỗi LED nối dây đen xuống hàng GND.

| Nút (trái → phải) | Chân Arduino | Việc |
|---|---|---|
| 1 | **7** | LED chẵn sáng |
| 2 | **6** | LED lẻ sáng |
| 3 | **5** | Đảo chẵn ↔ lẻ |
| 4 | 4 | Chưa dùng |

Mỗi nút: một chân nối dây đỏ lên hàng **+5V**, chân kia nối chân Arduino và qua điện trở kéo xuống **GND** (nhả đọc `LOW`, nhấn đọc `HIGH`). Nếu nối nút kiểu bản 01 (chân Arduino ↔ GND, không điện trở) thì đổi `PRESSED = LOW`, code tự chuyển sang `INPUT_PULLUP`.

> Chân đọc từ ảnh mạch Tinkercad. Với 4 LED, "chẵn/lẻ" theo vị trí (LED 2, 4) và theo số chân (12, 10) trùng nhau.
>
> ⚠️ **Mạch thật:** trong ảnh LED nối thẳng chân Arduino ↔ GND, chưa có điện trở. Thêm điện trở **220Ω** nối tiếp mỗi LED, nếu không LED và chân Arduino có thể hỏng.

### Prototype 11

Mạch Tinkercad bài 1.3 đủ 8 LED: 8 LED bên trái breadboard, 3 nút bên phải. Nguồn nối như bản 10 (5V, GND vào hàng nguồn dưới, hai dây bên trái nối lên hàng nguồn trên).

| LED (trái → phải) | Chân Arduino | Nhóm |
|---|---|---|
| 1 vàng | **13** | lẻ |
| 2 xanh dương | **12** | chẵn |
| 3 xanh lá | **11** | lẻ |
| 4 vàng | **10** | chẵn |
| 5 cam | **9** | lẻ |
| 6 trắng | **8** | chẵn |
| 7 xanh lá | **7** | lẻ |
| 8 xanh dương | **6** | chẵn |

Chân âm mỗi LED nối dây đen xuống hàng GND.

| Nút (trái → phải) | Chân Arduino | Việc |
|---|---|---|
| 1 | **4** | LED chẵn sáng |
| 2 | **3** | LED lẻ sáng |
| 3 | **2** | Đảo chẵn ↔ lẻ |

Mỗi nút nối như bản 10: một chân dây đỏ lên **+5V**, chân kia vào chân Arduino và qua điện trở kéo xuống **GND** (`PRESSED = HIGH`).

> Chân đọc từ ảnh mạch Tinkercad. "Chẵn/lẻ" theo vị trí (LED 2, 4, 6, 8) trùng với số chân (12, 10, 8, 6). Chân 13 còn nối với đèn L trên board nên đèn L sáng cùng LED 1.
>
> ⚠️ **Mạch thật:** trong ảnh LED vẫn chưa có điện trở. Thêm điện trở **220Ω** nối tiếp mỗi LED.

### Prototype 12

Mạch Tinkercad bài 1.4: 3 servo phía trên breadboard, 2 nút bên trái giữa Arduino và breadboard. Dây xanh lá từ **5V** và dây đen từ **GND** của Arduino nối vào hai hàng nguồn trên cùng của breadboard (+ và −); dây đỏ và dây đen của từng servo cắm vào hai hàng đó.

| Servo (trái → phải) | Dây tín hiệu | Chân Arduino | Nút 1 | Nút 2 |
|---|---|---|---|---|
| 1 | Xanh lá | **9** | 0° ↔ 90° | 0° ↔ 180° |
| 2 | Xanh lá | **10** | 0° ↔ 90° | 0° ↔ 180° |
| 3 | Xanh lá | **11** | 0° ↔ 180° | 0° ↔ 90° |

| Nút | Chân Arduino | Chân còn lại |
|---|---|---|
| 1 (nút dưới) | **3** (dây cam) | hàng **−** (GND, dây đen) |
| 2 (nút trên) | **2** (dây cam) | hàng **−** (GND, dây đen) |

Nút nối kiểu bản 01 (chân Arduino ↔ GND, không điện trở) nên code dùng `INPUT_PULLUP`, `PRESSED = LOW`. Dây cam và dây đen cắm vào **hai chân chéo nhau** của nút, cách nối này luôn đúng.

> Chân đọc từ ảnh mạch Tinkercad. Muốn đổi vai hai nút (nút trên là nút 1) thì sửa `BUTTON_PINS[] = {2, 3}`; muốn servo khác là servo quay "lẻ" thì sửa bảng `OUT_ANGLES`.
>
> ⚠️ **Mạch thật:** 3 servo cùng quay vượt 500 mA của cổng USB, Arduino dễ tự reset. Cấp hàng nguồn breadboard bằng nguồn 5V ≥ 2A riêng, nối **chung GND** với Arduino, bỏ dây 5V từ Arduino xuống hàng +.

### Prototype 13

Mạch Tinkercad: 2 nút và 2 LED trên breadboard, xếp từ trái sang phải. Dây đỏ từ **5V** và dây đen từ **GND** của Arduino nối vào hai hàng nguồn dưới cùng (+ và −).

| Linh kiện (trái → phải) | Chân Arduino | Cách nối | Nhấn đọc được |
|---|---|---|---|
| Nút 1 | **2** | Chân 2 qua điện trở kéo xuống **GND**, chân kia dây đỏ lên **+5V** | `HIGH` |
| Nút 2 | **3** | Chân 3 nối nút, chân kia dây đen xuống **GND**, không điện trở | `LOW` (code bật `INPUT_PULLUP`) |
| LED đỏ | **4** | Chân âm qua điện trở xuống **GND** | – |
| LED xanh | **5** | Chân âm qua điện trở xuống **GND** | – |

> Chân đọc từ ảnh mạch Tinkercad. Hai nút nối khác kiểu nên mảng `PRESSED[] = {HIGH, LOW}` ghi riêng mức nhấn của từng nút. Nếu nối lại nút 2 giống nút 1 (có điện trở kéo xuống, chân kia lên 5V) thì đổi thành `{HIGH, HIGH}`.

### Prototype 14

Servo không nối vào Arduino mà cắm vào mạch PCA9685; Arduino chỉ gửi lệnh qua I2C.

| PCA9685 | Nối tới | Dây trong ảnh |
|---|---|---|
| GND | **GND** Arduino | Đen |
| VCC | **5V** Arduino (chỉ nuôi chip) | Đỏ |
| SDA | **A4** | Xanh |
| SCL | **A5** | Vàng |
| Cầu đấu xanh V+ / GND | Nguồn riêng cho servo, **5V** | Đỏ / đen từ jack DC |

| Servo | Cắm vào cột **0** của PCA9685 |
|---|---|
| Dây cam (tín hiệu) | Hàng **PWM** (trên cùng) |
| Dây đỏ | Hàng **V+** (giữa) |
| Dây nâu | Hàng **GND** (dưới cùng) |

> Chân đọc từ ảnh sơ đồ. "Kênh 0" là cột 0 trên hàng chân servo. Các cầu hàn A0–A5 ở góc mạch dùng để đổi địa chỉ I2C, không liên quan tới servo, cứ để nguyên.
>
> ⚠️ **Mạch thật:** cầu đấu xanh chịu **tối đa 6V**. Không cắm adapter 9–12V vào đây, servo và mạch sẽ hỏng. Dùng nguồn 5V ≥ 1A, đúng cực + và −. Không lấy nguồn servo từ chân 5V của Arduino. GND của nguồn servo và GND của Arduino đã nối chung qua mạch PCA9685 nhờ dây GND ở bảng trên, nên không được bỏ dây đó.

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
- Bản 09: chỉ giữ hiệu ứng đồng bộ của bản 08. Hàm `angleAt(t)` tính một góc chung theo thời gian trong vòng (lượt lên 2,7 giây, dừng 0,5 giây, lượt về 2,7 giây, dừng 0,5 giây; một vòng 6,4 giây) và `writeAll()` gửi góc đó cho cả 5 servo trong cùng một lượt, chỉ khi góc thay đổi. Chỉnh tốc độ bằng `DONG_BO_MOVE_MS`, thời gian dừng bằng `DONG_BO_HOLD_MS`.
- Bản 10: mỗi nút có bộ chống dội 50 ms riêng (mảng `lastReading[]`, `stableState[]`, `lastChangeMs[]`), chỉ tính lúc nhấn xuống nên giữ nút không lặp lại. Biến `mode` nhớ chế độ `TAT_HET` / `CHAN` / `LE`; nút 3 đổi `CHAN` → `LE`, còn lại → `CHAN` (lúc bật nguồn mọi LED tắt, nhấn nút 3 thì chẵn sáng trước). Hàm `ledOnIn(mode, i)` quyết định LED thứ i sáng hay tắt, nên lên 8 LED chỉ cần thêm chân vào `LED_PINS[]`.
- Bản 11: giống hệt bản 10, chỉ đổi `LED_PINS[] = {13, 12, 11, 10, 9, 8, 7, 6}` và `BUTTON_PINS[] = {4, 3, 2}` theo mạch 8 LED. LED thứ i (đếm từ 0) là LED chẵn khi i lẻ.
- Bản 12: chống dội nút giống bản 10. Nhấn một lần là chạy liên tục, không cần giữ. Bảng `OUT_ANGLES[nút][servo]` ghi góc quay tới của từng servo theo từng nút; mỗi vòng dài `2 × STEP_MS`: nửa đầu ở góc quay tới, nửa sau về `REST_ANGLE` (0°). Hàm `angleAt(mode, i, t)` tính góc theo thời gian từ đầu vòng, chỉ gửi lệnh khi góc đổi. Servo nhận lệnh ghi thẳng góc đích và tự quay hết tốc độ (SG90 cần ~0,3 giây cho 180°, nên 0,5 giây là đủ tới nơi và đứng lại). Đổi nút: đang ở góc quay tới thì về 0° ngay, đang ở 0° thì đứng nốt cho đủ 0,5 giây, nên servo luôn đứng ở 0° đủ 0,5 giây trước khi quay góc mới. Nhấn lại nút đang chạy thì giữ nguyên nhịp.
- Bản 13: giống bản 01 nhưng cho 2 cặp nút–LED: nút thứ b bật/tắt LED thứ b (`BUTTON_PINS[b]` → `LED_PINS[b]`), mỗi nút có bộ chống dội 50 ms riêng như bản 10. Mảng `PRESSED[]` cho từng nút một mức nhấn: `HIGH` thì `pinMode(INPUT)`, `LOW` thì `pinMode(INPUT_PULLUP)`.
- Bản 14: PCA9685 phát xung 50 Hz, mỗi chu kỳ chia 4096 bước; `pulseTicks()` đổi góc thành số bước, dùng dải 544–2400 µs giống `Servo.h`. Chương trình quét đều `QUET_ROUNDS` vòng (lượt 2,7 giây, dừng 0,5 giây) rồi đi từng nấc `NAC_ROUNDS` vòng (4 nấc 45°, mỗi nấc quay 0,4 giây, dừng 0,6 giây), mặc định 2 + 1 vòng, dài 20,8 giây. Góc là hàm của thời gian (`angleQuet()`, `angleNac()`), chỉ gửi I2C khi góc đổi. Không thấy PCA9685 lúc khởi động thì đèn L nháy nhanh và không gửi lệnh. Đổi kênh bằng `SERVO_CHANNEL`.

## Cách chạy

**Wokwi:** dán `sketch.ino` vào tab sketch. Với bản 02, tạo thêm tab `libraries.txt` và dán nội dung file đó để Wokwi tự cài thư viện LCD.

**Tinkercad:** bấm **Code** (Mã), chuyển sang chế độ **Text** (Văn bản), xóa code cũ rồi dán `sketch.ino`. Thư viện `Servo.h` có sẵn, không cần cài.

**Arduino IDE:** với bản 02, cài thư viện **LiquidCrystal I2C** (Frank de Brabander) trong Library Manager. Với bản 14, cài **Adafruit PWM Servo Driver Library** (chọn cài kèm Adafruit BusIO).

**Bản 14 cần mạch thật:** Tinkercad không có linh kiện PCA9685, nên nạp `sketch.ino` bằng Arduino IDE.

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
- Bản 10 nhấn nút không có tác dụng hoặc LED tự đổi lung tung: kiểm tra kiểu nối nút. Nút có điện trở kéo xuống GND và chân kia lên 5V thì `PRESSED = HIGH`; nút nối thẳng chân Arduino ↔ GND thì `PRESSED = LOW`.
- Nút Tinkercad nhấn không ăn hoặc như bị nhấn liên tục: hai dây đang cắm vào hai chân luôn nối với nhau bên trong nút. Chuyển một dây sang chân chéo đối diện.
- Tinkercad/Arduino IDE báo `#error "Đây là file test chạy trên máy tính..."`, hoặc `No such file or directory` ở một dòng `#include` (`string`, `vector`, `../prototype-NN-.../sketch.ino`): đang dán nhầm file test trong `tests/`. File test chỉ chạy trên máy tính bằng `g++`; code dán vào mạch luôn là `prototype-NN-.../sketch.ino`.
- Bản 13 một nút ăn, nút kia không ăn hoặc đèn tự bật/tắt: mức nhấn trong `PRESSED[]` không khớp kiểu nối của nút đó. Nút nối thẳng chân ↔ GND mà để `HIGH` thì chân bị thả nổi, đọc lung tung.
- Bản 12 servo không quay hết 180° trước khi quay về: `STEP_MS` quá nhỏ (dưới ~350 ms với SG90), tăng lại.
- Bản 14 đèn L nháy nhanh: Arduino không thấy PCA9685. Kiểm tra VCC, GND và xem SDA có vào A4, SCL có vào A5 không (hay bị đảo). Nếu đã hàn cầu địa chỉ thì sửa `PCA_ADDRESS`.
- Bản 14 đèn L không nháy nhưng servo đứng yên: chưa cấp nguồn vào cầu đấu xanh, hoặc servo cắm ngược hàng (dây cam phải ở hàng PWM) hay cắm sai cột.
- Bản 14 servo không đủ 180° hoặc kêu rè ở hai đầu: dao động của PCA9685 lệch theo từng mạch, chỉnh `SERVO_MIN_US`, `SERVO_MAX_US`.
