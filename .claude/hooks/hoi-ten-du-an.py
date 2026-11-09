"""Hook PreToolUse cho Write: bắt xác nhận trước khi tạo một file .ino mới.

Quy tắc của dự án: phải hỏi tên dự án trước khi xuất file code, tránh trùng lặp sketch.
Sửa file .ino đã có thì cho qua, vì sửa lỗi phải làm ngay trên file cũ.
"""
import json
import os
import sys

data = json.loads(sys.stdin.buffer.read().decode("utf-8"))
path = data.get("tool_input", {}).get("file_path", "")

if not path.lower().endswith(".ino"):
    sys.exit(0)
if os.path.exists(os.path.join(data.get("cwd", ""), path)):
    sys.exit(0)

reason = (
    "Sắp tạo sketch mới: " + os.path.basename(os.path.dirname(path)) + ". "
    "Quy tắc dự án: phải hỏi tên dự án trước khi xuất file code. "
    "Chỉ đồng ý nếu tên này do bạn đặt."
)
# ensure_ascii giữ stdout an toàn với bảng mã mặc định của Windows
print(json.dumps({
    "hookSpecificOutput": {
        "hookEventName": "PreToolUse",
        "permissionDecision": "ask",
        "permissionDecisionReason": reason,
    }
}))
