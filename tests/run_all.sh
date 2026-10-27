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

echo; echo "== 4. Biên dịch thật cho Arduino Uno (arduino-cli) =="
bash tests/compile_uno.sh || fail=1

echo
if [ $fail -eq 0 ]; then echo "TẤT CẢ ĐẠT"; else echo "CÓ LỖI, xem ở trên"; fi
exit $fail
