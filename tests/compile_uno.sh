#!/usr/bin/env bash
# Biên dịch thật mọi sketch cho Arduino Uno (ATmega328P) bằng arduino-cli, in dung lượng Flash/RAM.
# Bắt lỗi mà bản giả lập g++ bỏ sót: thiếu thư viện, int 16 bit, tràn RAM 2 KB.
# Chạy từ thư mục gốc dự án: bash tests/compile_uno.sh
# Lần đầu cần: arduino-cli core install arduino:avr && arduino-cli lib install Servo "LiquidCrystal I2C"
set -u
cd "$(dirname "$0")/.."

CLI=$(command -v arduino-cli || ls "/c/Program Files/Arduino CLI/arduino-cli.exe" 2>/dev/null)
if [ -z "$CLI" ]; then echo "BỎ QUA  chưa cài arduino-cli"; exit 0; fi

fail=0
out=$(mktemp)
for f in prototype-*/sketch.ino; do
  name=$(dirname "$f")
  # arduino-cli bắt tên thư mục trùng tên file .ino, nên chép ra build/uno/<prototype>/sketch/
  dir="build/uno/$name/sketch"
  mkdir -p "$dir" && cp "$f" "$dir/sketch.ino"
  if "$CLI" compile --fqbn arduino:avr:uno --warnings all "$dir" >"$out" 2>&1; then
    flash=$(grep -o 'uses [0-9]* bytes ([0-9]*%) of program' "$out" | grep -o '([0-9]*%)')
    ram=$(grep -o 'use [0-9]* bytes ([0-9]*%) of dynamic' "$out" | grep -o '([0-9]*%)')
    printf 'OK    %-42s Flash %-6s RAM %s\n' "$name" "$flash" "$ram"
    # Chỉ in cảnh báo trong sketch (bỏ cảnh báo của thư viện), đổi đường dẫn bản chép về file gốc
    grep 'sketch\.ino:[0-9]*:[0-9]*: warning:' "$out" | sed 's#^.*sketch\.ino:#      '"$name"'/sketch.ino:#'
  else
    echo "LỖI   $name"; cat "$out"; fail=1
  fi
done
rm -f "$out"
exit $fail
