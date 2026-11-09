---
name: arduino-toi-uu
description: Tối ưu sketch Arduino Uno trong dự án adruino – đo Flash/RAM trước và sau, giảm bộ nhớ, bỏ code lặp, sửa chỗ làm chậm hoặc giật mà không đổi hành vi. Dùng khi người dùng nói "tối ưu", "rút gọn code", "giảm RAM/Flash", "code nặng", "chạy chậm", "gọn lại bản NN", hoặc khi RAM vượt 75 %.
---

# Tối ưu sketch Arduino Uno

Trả lời bằng tiếng Việt. Quy ước code nằm trong `CLAUDE.md`. Tối ưu là **sửa ngay trên file của bản đó**, không tạo thư mục hay bản sao mới.

## 1. Đo trước khi sửa

```bash
bash tests/compile_uno.sh
```

Ghi lại Flash/RAM của bản cần tối ưu. Giới hạn của Uno: Flash 32 256 byte, RAM 2 048 byte.

Chỉ giảm bộ nhớ khi có lý do thật: RAM trên 75 %, Flash trên 85 %, hoặc sắp thêm thư viện lớn. Các bản hiện tại chỉ dùng Flash 3–11 %, RAM tối đa 15 %, nên thường nên ưu tiên **dễ đọc** và **bỏ code lặp** hơn là ép từng byte. Nếu không có gì đáng tối ưu thì nói thẳng với người dùng.

## 2. Các cách giảm, xếp theo hiệu quả trên Uno

| Vấn đề | Cách làm |
|---|---|
| Chuỗi cố định chiếm RAM | `Serial.print(F("..."))`, `lcd.print(F("..."))` |
| Dùng `String` (phân mảnh heap 2 KB) | Dùng `char[]` hoặc in từng phần |
| `float` kéo theo thư viện số thực (vài KB Flash) | Tính bằng số nguyên, `long` ở bước trung gian, chia sau cùng |
| Biến to hơn cần thiết | `byte` cho chân, góc, PWM; `unsigned long` chỉ cho thời gian |
| Bảng lớn (từ vài chục phần tử) trong RAM | `const ... PROGMEM` + `pgm_read_byte()`; mảng chân nhỏ thì giữ nguyên |
| Code chép lại theo từng chân | Gom thành hàm + mảng `*_PINS[]`, số lượng tính bằng `sizeof` |
| Ghi ra chân liên tục | Chỉ ghi khi đổi giá trị (`lastPwm[]`, `lastAngle[]`) |

## 3. Lỗi tiềm ẩn cần sửa khi gặp

- `int` chỉ 16 bit: nhân thời gian hoặc góc lớn thì ép `(long)` trước khi nhân.
- So sánh thời gian phải viết `millis() - moc >= KHOANG`, không viết `millis() >= moc + KHOANG` (sai khi `millis()` tràn sau 49,7 ngày).
- Biến dùng chung với ngắt phải có `volatile`.

## 4. Không làm

- Không dùng `delay()`, `static_assert`, `constexpr` để "tối ưu" (trái quy ước, Tinkercad có thể không hỗ trợ).
- Không đổi `digitalWrite` sang ghi cổng trực tiếp nếu không cần tốc độ cỡ kHz: khó đọc và mất tính dễ đổi chân.
- Không đổi hành vi: thời gian, góc, thứ tự chân, số vòng hiệu ứng phải giữ nguyên.
- Không gộp hiệu ứng hay bỏ thông số chỉnh được ở đầu file chỉ để code ngắn hơn.

## 5. Kiểm tra và báo kết quả

```bash
bash tests/run_all.sh
```

Phải ra **TẤT CẢ ĐẠT**. Báo bảng Flash/RAM trước và sau cho bản đã sửa, cùng các thay đổi chính. Thay đổi không giảm được gì đáng kể mà làm code khó đọc hơn thì hoàn tác. Đề nghị commit (chỉ commit khi người dùng đồng ý).

Có thể dùng thêm `/simplify` để rà code lặp và `/code-review` để tìm lỗi trên phần vừa sửa.
