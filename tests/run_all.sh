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

# Bản 17: chỉ thay bản sinh trong build/, không sửa sketch gốc.
if python -I - <<'PY'
import pathlib
import re

source = pathlib.Path("prototype-17-dong-tac-gap-do/sketch.ino").read_text(encoding="utf-8")

def replace_once(text, pattern, replacement, name):
    result, count = re.subn(pattern, replacement, text, flags=re.MULTILINE)
    if count != 1:
        raise SystemExit(f"LỖI: cần tìm đúng một khai báo {name} để tạo biến thể 17")
    return result


zero_gap = replace_once(
    source, r"^(const unsigned long SET_GAP_MS\s*=\s*)[^;\n]+;",
    r"\g<1>0;", "SET_GAP_MS")


def zero_speeds(match):
    # Giữ nguyên số hàng/cột, đặt TẤT CẢ tốc độ (kể cả hàng về base) về 0.
    body = re.sub(r"//[^\n]*|/\*[\s\S]*?\*/", "", match.group(2))
    rows = re.findall(r"\{([^{}]*)\}", body)
    if not rows or re.sub(r"\{[^{}]*\}|[\s,]", "", body):
        raise SystemExit("LỖI: không đọc được các hàng TOC_DO_DEG_S[][]")
    zero_rows = []
    for row in rows:
        values = [value.strip() for value in row.split(",") if value.strip()]
        if not values or any(not re.fullmatch(r"\d+[uUlL]*", value) for value in values):
            raise SystemExit("LỖI: không đọc được mọi phần tử TOC_DO_DEG_S[][]")
        zero_rows.append("  {" + ", ".join("0" for _ in values) + "},")
    return match.group(1) + "{\n" + "\n".join(zero_rows) + "\n};"


speed_pattern = r"^(const unsigned long\s+TOC_DO_DEG_S\s*\[\s*\]\s*\[\s*SERVO_COUNT\s*\]\s*=\s*)\{([\s\S]*?)^\};"
variants = {
    "zero_gap": zero_gap,
    "zero_speed": replace_once(source, speed_pattern, zero_speeds, "TOC_DO_DEG_S[][]"),
    "zero_both": replace_once(zero_gap, speed_pattern, zero_speeds, "TOC_DO_DEG_S[][]"),
}

# Nhóm đầu có dòng dài nhất ở đầu, để bắt lỗi chỉ chờ dòng cuối.
# Hai lần lặp servo CUNG_LUC thử nghỉ 500 ms và 1500 ms; cuối bảng mọi servo lệch base.
tables = {
    "custom_table": """
  {1, 1, 150, LAN_LUOT},
  {1, 2, 35, CUNG_LUC},
  {1, 3, 50, CUNG_LUC},
  {2, 4, 30, LAN_LUOT},
  {2, 4, 60, CUNG_LUC},
  {2, 2, 75, CUNG_LUC},
  {3, 1, 120, CUNG_LUC},
  {3, 2, 95, CUNG_LUC},
  {3, 3, 90, CUNG_LUC},
""",
    "empty_table": "",
    "invalid_table": """
  {1, 0, 180, LAN_LUOT},
  {1, 255, 0, CUNG_LUC},
  {1, 5, 180, LAN_LUOT},
  {0, 1, 180, LAN_LUOT},
  {4, 2, 180, CUNG_LUC},
  {1, 1, 135, LAN_LUOT},
  {1, 0, 0, CUNG_LUC},
  {0, 1, 0, LAN_LUOT},
  {4, 2, 0, LAN_LUOT},
  {2, 2, 45, CUNG_LUC},
  {1, 255, 0, LAN_LUOT},
  {255, 3, 180, LAN_LUOT},
  {3, 3, 60, LAN_LUOT},
  {1, 5, 0, CUNG_LUC},
  {0, 4, 180, CUNG_LUC},
  {4, 4, 180, LAN_LUOT},
""",
}
table_pattern = r"^(const BuocQuay DONG_TAC_1\[\]\s*=\s*)\{[\s\S]*?^\};"
for suffix, rows in tables.items():
    variants[suffix] = replace_once(
        source, table_pattern, lambda match: match.group(1) + "{" + rows + "\n};",
        "DONG_TAC_1[]")

# Mọi hàng/cột khác nhau: lấy nhầm bước, servo hoặc hàng về base đều đổi lịch.
custom_speeds = """{
  {5, 10, 20, 25},
  {40, 50, 100, 125},
  {200, 250, 500, 1000},
  {1, 2, 4, 8},
};"""
variants["custom_table"] = replace_once(
    variants["custom_table"], speed_pattern,
    lambda match: match.group(1) + custom_speeds, "TOC_DO_DEG_S[][]")

for suffix, variant in variants.items():
    pathlib.Path(f"build/test_17_{suffix}.inc").write_text(variant, encoding="utf-8")
PY
then
  for suffix in zero_gap zero_speed zero_both custom_table empty_table invalid_table; do
    case "$suffix" in
      zero_gap) macro=TEST_17_ZERO_GAP; label="SET_GAP_MS=0" ;;
      zero_speed) macro=TEST_17_ZERO_SPEED; label="mọi TOC_DO_DEG_S[][]=0" ;;
      zero_both) macro=TEST_17_ZERO_BOTH; label="SET_GAP_MS=0, mọi TOC_DO_DEG_S[][]=0" ;;
      custom_table) macro=TEST_17_CUSTOM_TABLE; label="tốc độ riêng từng hàng/cột, nhóm cùng lúc, lặp servo, tự về cả 4 base" ;;
      empty_table) macro=TEST_17_EMPTY_TABLE; label="bảng bước rỗng" ;;
      invalid_table) macro=TEST_17_INVALID_TABLE; label="bỏ qua số servo 0/5/255 và bước 0/4/255" ;;
    esac
    echo "-- test_17_dong_tac_gap_do ($label)"
    if g++ -std=gnu++11 -O1 -Itests/mock "-D$macro" tests/test_17_dong_tac_gap_do.cpp tests/mock/globals.cpp -o "build/test_17_$suffix"; then
      "./build/test_17_$suffix" || fail=1
    else
      fail=1
    fi
  done
else
  fail=1
fi

echo; echo "== 4. Biên dịch thật cho Arduino Uno (arduino-cli) =="
bash tests/compile_uno.sh || fail=1

echo
if [ $fail -eq 0 ]; then echo "TẤT CẢ ĐẠT"; else echo "CÓ LỖI, xem ở trên"; fi
exit $fail
