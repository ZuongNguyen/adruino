# Kiểm tra chân trong mọi sketch trước khi dán vào Wokwi/Tinkercad hay nạp vào mạch thật.
# Chạy từ thư mục gốc dự án: python -I tests/check_pins.py
#
# Bắt các lỗi phần cứng dễ gặp trên Arduino Uno:
#   - analogWrite() trên chân không có PWM (chỉ 3, 5, 6, 9, 10, 11 có dấu ~)
#   - analogWrite() trên chân 9, 10 khi có Servo.h (Servo chiếm Timer1)
#   - một chân bị dùng cho hai việc
#   - dùng chân 0, 1 (Serial) hoặc A4, A5 khi đã có LCD I2C
#   - quá 12 servo
import re
import sys
from pathlib import Path

PWM_PINS = {3, 5, 6, 9, 10, 11}
TIMER1_PINS = {9, 10}
MAX_SERVOS = 12
PIN_ALIAS = {"A0": 14, "A1": 15, "A2": 16, "A3": 17, "A4": 18, "A5": 19}

RE_ONE = re.compile(r"const\s+(?:uint8_t|int|byte)\s+(\w*PIN)\s*=\s*(\w+)\s*;")
RE_ARR = re.compile(r"const\s+(?:uint8_t|int|byte)\s+(\w*PINS)\s*\[\s*\]\s*=\s*\{([^}]*)\}")


def to_pin(text):
    text = text.strip()
    return PIN_ALIAS[text] if text in PIN_ALIAS else int(text)


def pin_name(p):
    for k, v in PIN_ALIAS.items():
        if v == p:
            return k
    return str(p)


def check(path):
    code = re.sub(r"//.*", "", path.read_text(encoding="utf-8"))
    pins = {}                                   # tên hằng -> danh sách chân
    for name, val in RE_ONE.findall(code):
        pins[name] = [to_pin(val)]
    for name, vals in RE_ARR.findall(code):
        pins[name] = [to_pin(v) for v in vals.split(",") if v.strip()]

    has_servo = "#include <Servo.h>" in code
    has_i2c = "#include <Wire.h>" in code
    errors, notes = [], []

    used = {}                                   # chân -> tên hằng đã dùng nó
    for name, lst in pins.items():
        for p in lst:
            if p in used:
                errors.append(f"chân {pin_name(p)} dùng cho cả {used[p]} và {name}")
            used[p] = name
            if p in (0, 1):
                notes.append(f"{name} dùng chân {p}, trùng Serial (cổng USB)")
            if has_i2c and p in (18, 19):
                errors.append(f"{name} dùng {pin_name(p)}, trùng SDA/SCL của LCD I2C")

    for name, lst in pins.items():
        if re.search(r"analogWrite\(\s*" + name + r"\b", code):
            for p in lst:
                if p not in PWM_PINS:
                    errors.append(f"analogWrite() trên chân {pin_name(p)} ({name}) không có PWM")
                elif has_servo and p in TIMER1_PINS:
                    errors.append(f"analogWrite() trên chân {p} ({name}) không chạy khi có Servo.h (Timer1)")

    servo_count = sum(len(lst) for name, lst in pins.items() if "SERVO" in name)
    if servo_count > MAX_SERVOS:
        errors.append(f"{servo_count} servo, Uno chỉ chạy tối đa {MAX_SERVOS}")
    if servo_count >= 2:
        notes.append(f"{servo_count} servo: mạch thật cần nguồn 5V riêng, nối chung GND")

    return pins, errors, notes


def main():
    root = Path(__file__).resolve().parent.parent
    fail = False
    for path in sorted(root.glob("prototype-*/sketch.ino")):
        pins, errors, notes = check(path)
        summary = ", ".join(f"{n}={'/'.join(pin_name(p) for p in l)}" for n, l in pins.items())
        print(("LỖI  " if errors else "OK   ") + f"{path.parent.name}  [{summary}]")
        for e in errors:
            print("      ✗ " + e)
        for n in notes:
            print("      · " + n)
        fail = fail or bool(errors)
    return 1 if fail else 0


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8")
    sys.exit(main())
