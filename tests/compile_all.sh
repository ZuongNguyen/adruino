#!/usr/bin/env bash
# Biên dịch thử mọi sketch với Arduino giả lập (g++) để bắt lỗi cú pháp trước khi dán vào Wokwi/Tinkercad.
# Chạy từ thư mục gốc dự án: bash tests/compile_all.sh
set -u
cd "$(dirname "$0")/.."
fail=0
err=$(mktemp)
for f in prototype-*/sketch.ino; do
  if g++ -std=gnu++11 -fsyntax-only -Wall -Wextra -Itests/mock -include Arduino.h -x c++ "$f" 2>"$err"; then
    echo "OK    $f"
  else
    echo "LỖI   $f"; cat "$err"; fail=1
  fi
done
rm -f "$err"
exit $fail
