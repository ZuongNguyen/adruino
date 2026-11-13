#!/usr/bin/env bash
# Giao việc kiểm tra lỗi và thử nghiệm cho ChatGPT (Codex CLI). Claude chạy sau khi code xong trong worktree.
# Dùng: bash scripts/chatgpt-kiem-tra.sh "<Claude vừa làm gì, bản nào>"
# Kết quả: .chatgpt/bao-cao.md (báo cáo theo mẫu trong AGENTS.md), .chatgpt/nhat-ky.txt (toàn bộ quá trình).
# Mã thoát: 0 = ĐẠT, 1 = CHƯA ĐẠT hoặc báo cáo không rõ, 2 = không chạy được ChatGPT.
set -u
cd "$(git rev-parse --show-toplevel)" || exit 2

# App Codex để codex.exe trong thư mục mang mã phiên bản, đổi sau mỗi lần cập nhật: lấy file mới nhất
CODEX=$(command -v codex || ls -t "$HOME"/AppData/Local/OpenAI/Codex/bin/*/codex.exe 2>/dev/null | head -1)
if [ -z "$CODEX" ]; then
  echo "Không tìm thấy codex.exe. Mở app Codex (ChatGPT) một lần cho nó cài lại rồi chạy lại."
  exit 2
fi

mkdir -p .chatgpt
rm -f .chatgpt/bao-cao.md
viec=${1:-"(Claude không ghi)"}
nhanh=$(git branch --show-current)

echo "ChatGPT đang kiểm tra nhánh ${nhanh:-?} (thường mất vài phút)..."
# Sandbox của Codex trên Windows không đọc được g++ (WinLibs) và Arduino15 trong hồ sơ người dùng.
# --approve-for-me: sandbox workspace-write (tự bật, không đi kèm -s được); ChatGPT xin chạy tests/run_all.sh
# ngoài sandbox, bộ duyệt tự động của Codex quyết định.
# Không dùng --add-dir: nó cấp quyền ghi vĩnh viễn (ACL) cho thư mục ngoài dự án.
# Lời giao việc đi qua stdin để giữ nguyên tiếng Việt có dấu.
"$CODEX" exec -C "$(pwd -W)" --approve-for-me --color never \
  -o "$(pwd -W)/.chatgpt/bao-cao.md" - > .chatgpt/nhat-ky.txt 2>&1 <<EOF
Bạn là người kiểm tra lỗi và thử nghiệm của dự án này. Đọc AGENTS.md rồi làm đúng các bước và giới hạn ở đó.
Nhánh: ${nhanh:-không rõ}
Việc Claude vừa làm: $viec
Tin nhắn cuối cùng của bạn phải là báo cáo đúng mẫu trong AGENTS.md, dòng đầu là "KẾT LUẬN: ĐẠT" hoặc "KẾT LUẬN: CHƯA ĐẠT".
EOF
ma=$?

if [ $ma -ne 0 ] || [ ! -s .chatgpt/bao-cao.md ]; then
  echo "ChatGPT không chạy xong (mã $ma). Cuối nhật ký:"
  tail -n 20 .chatgpt/nhat-ky.txt
  exit 2
fi

cat .chatgpt/bao-cao.md
echo
# Dòng có chữ đầu tiên, bỏ dấu in đậm/backtick nếu ChatGPT lỡ thêm
dau=$(grep -m 1 -v '^[[:space:]]*$' .chatgpt/bao-cao.md | tr -d '*`')
if [[ "$dau" == "KẾT LUẬN: ĐẠT"* ]]; then
  echo "== ChatGPT: ĐẠT =="
  exit 0
fi
echo "== ChatGPT: CHƯA ĐẠT hoặc báo cáo không rõ, xem .chatgpt/bao-cao.md =="
exit 1
