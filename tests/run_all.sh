#!/usr/bin/env bash
# Chạy toàn bộ kiểm tra: chân -> biên dịch giả lập -> test logic -> biên dịch thật cho Uno.
# Chạy từ thư mục gốc dự án: bash tests/run_all.sh
set -u
cd "$(dirname "$0")/.."
fail=0

# Shell mở trước khi cài g++ (winget WinLibs) chưa có nó trong PATH
if ! command -v g++ >/dev/null; then
  for d in /c/Users/*/AppData/Local/Microsoft/WinGet/Packages/BrechtSanders.WinLibs.*/mingw64/bin; do
    [ -x "$d/g++.exe" ] && export PATH="$d:$PATH" && break
  done
fi

echo "== 1. Kiểm tra chân =="
python -I tests/check_pins.py || fail=1

echo; echo "== 2. Biên dịch giả lập (g++) =="
bash tests/compile_all.sh || fail=1

echo; echo "== 3. Test logic theo thời gian =="
mkdir -p build
for t in tests/test_*.cpp; do
  name=$(basename "$t" .cpp)
  echo "-- $name"
  if g++ -std=gnu++11 -O1 -Itests/mock "$t" tests/mock/globals.cpp -o "build/$name"; then
    "./build/$name" || fail=1
  else
    fail=1
  fi
done

# Kiểm tra cấu hình gap 0 và tốc độ 0 bằng đúng sketch/test 16, chỉ đổi bản sinh ở build/.
if python -I - <<'PY'
import pathlib
import re

source = pathlib.Path("prototype-16-reset-5-servo/sketch.ino").read_text(encoding="utf-8")
for constant, suffix in (("RESET_GAP_MS", "zero_gap"), ("MOVE_SPEED_DEG_S", "zero_speed")):
    variant, count = re.subn(
        rf"^(const unsigned long {constant}\s*=\s*)[^;\n]+;",
        r"\g<1>0;", source, flags=re.MULTILINE)
    if count != 1:
        raise SystemExit(f"LỖI: cần tìm đúng một khai báo {constant} để tạo biến thể {suffix}")
    pathlib.Path(f"build/test_16_{suffix}.inc").write_text(variant, encoding="utf-8")
PY
then
  echo "-- test_16_reset_5_servo (RESET_GAP_MS=0)"
  if g++ -std=gnu++11 -O1 -Itests/mock -DTEST_16_ZERO_GAP tests/test_16_reset_5_servo.cpp tests/mock/globals.cpp -o build/test_16_zero_gap; then
    ./build/test_16_zero_gap || fail=1
  else
    fail=1
  fi
  echo "-- test_16_reset_5_servo (MOVE_SPEED_DEG_S=0)"
  if g++ -std=gnu++11 -O1 -Itests/mock -DTEST_16_ZERO_SPEED tests/test_16_reset_5_servo.cpp tests/mock/globals.cpp -o build/test_16_zero_speed; then
    ./build/test_16_zero_speed || fail=1
  else
    fail=1
  fi
else
  fail=1
fi

echo; echo "== 4. Biên dịch thật cho Arduino Uno (arduino-cli) =="
bash tests/compile_uno.sh || fail=1

echo
if [ $fail -eq 0 ]; then echo "TẤT CẢ ĐẠT"; else echo "CÓ LỖI, xem ở trên"; fi
exit $fail
