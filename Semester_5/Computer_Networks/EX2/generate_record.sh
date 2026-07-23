#!/usr/bin/env bash
# ──────────────────────────────────────────────────────────────
# generate_record.sh — EX2 Lab Record Generator
# Compiles & runs every .c / .cpp / .java file, records output
# to EX2.prn using macOS `script` + a Python PTY wrapper so
# stdin echo appears exactly as if a human typed it.
# ──────────────────────────────────────────────────────────────

set -euo pipefail

EX_NAME="EX2"
PRN_FILE="${EX_NAME}.prn"
PROMPT="abishekvh@Abisheks-MacBook-Air $(basename "$PWD") %"

# ─── Input Map ────────────────────────────────────────────────
get_inputs() {
    local file="$1"
    case "$file" in

        # ── Root directory programs ──

        Main.c)
            # Menu: 5 = Run Network Layers
            # Source URL, Dest URL
            # 3 frames → inject error prompts: n, y, n
            # Then menu: 6 = Exit
            cat <<'EOF'
5
www.google.com
www.youtube.com
n
y
n
6
EOF
            ;;

        # ── Excercise Questions programs ──

        BitwiseOperatorsProgram.c)
            # scanf("%d %d", &n, &k)  →  n=5 k=4
            cat <<'EOF'
5 4
EOF
            ;;

        AlternatingBitsProgram.java)
            # Enter an integer: 5
            cat <<'EOF'
5
EOF
            ;;

        BinaryAdditionProgram.java)
            # Enter first binary string:  1010
            # Enter second binary string: 1011
            cat <<'EOF'
1010
1011
EOF
            ;;

        SubarrayOrsProgram.java)
            # Enter array size: 3
            # Enter array elements: 1 2 3
            cat <<'EOF'
3
1 2 3
EOF
            ;;

        *)
            echo ""
            ;;
    esac
}

# ─── Python PTY runner ────────────────────────────────────────
run_with_pty() {
    local cmd="$1"
    local inputs="$2"

    python3 - "$cmd" "$inputs" <<'PYEOF'
import sys, os, select, time, pty, subprocess

cmd    = sys.argv[1]
inputs = sys.argv[2].strip("\n").split("\n") if sys.argv[2].strip() else []

master_fd, slave_fd = pty.openpty()

proc = subprocess.Popen(
    cmd, shell=True,
    stdin=slave_fd, stdout=slave_fd, stderr=slave_fd,
    close_fds=True
)
os.close(slave_fd)

def read_all(fd, timeout=0.3):
    buf = b""
    end = time.time() + timeout
    while True:
        remain = end - time.time()
        if remain <= 0:
            break
        rlist, _, _ = select.select([fd], [], [], remain)
        if fd in rlist:
            try:
                chunk = os.read(fd, 4096)
            except OSError:
                break
            if not chunk:
                break
            buf += chunk
            end = time.time() + timeout
        else:
            break
    return buf

sys.stdout.buffer.write(read_all(master_fd).replace(b"\r\n", b"\n"))
sys.stdout.buffer.flush()

for line in inputs:
    os.write(master_fd, (line + "\n").encode())
    time.sleep(0.15)
    data = read_all(master_fd, timeout=0.5)
    sys.stdout.buffer.write(data.replace(b"\r\n", b"\n"))
    sys.stdout.buffer.flush()

proc.wait()
try:
    leftover = read_all(master_fd, timeout=0.5)
    sys.stdout.buffer.write(leftover.replace(b"\r\n", b"\n"))
    sys.stdout.buffer.flush()
except OSError:
    pass

os.close(master_fd)
sys.exit(proc.returncode)
PYEOF
}

# ─── Process a single source file ─────────────────────────────
process_file() {
    local src="$1"
    local base
    base="$(basename "$src")"
    local ext="${base##*.}"

    local inputs
    inputs="$(get_inputs "$base")"

    echo ""
    echo "${PROMPT} cat ${base}"
    echo ""
    cat "$src"
    echo ""

    if [[ "$ext" == "c" ]]; then
        echo "${PROMPT} gcc ${base}"
        gcc "$src" -o a.out 2>&1
        echo "${PROMPT} ./a.out"
        run_with_pty "./a.out" "$inputs"
        echo ""

    elif [[ "$ext" == "cpp" ]]; then
        echo "${PROMPT} g++ ${base}"
        g++ "$src" -o a.out 2>&1
        echo "${PROMPT} ./a.out"
        run_with_pty "./a.out" "$inputs"
        echo ""

    elif [[ "$ext" == "java" ]]; then
        local classname="${base%.java}"
        echo "${PROMPT} javac ${base}"
        javac "$src" 2>&1
        echo "${PROMPT} java ${classname}"
        run_with_pty "java -cp '$(dirname "$src")' ${classname}" "$inputs"
        echo ""
    fi
}

# ─── Process a multi-file C program ──────────────────────────
process_multifile() {
    local main_src="$1"
    shift
    local extra_srcs=("$@")

    local all_display=""
    local all_compile=""
    for f in "$main_src" "${extra_srcs[@]}"; do
        local b
        b="$(basename "$f")"
        all_display+=" ${b}"
        all_compile+=" ${f}"
    done
    all_display="${all_display# }"
    all_compile="${all_compile# }"

    local base
    base="$(basename "$main_src")"
    local inputs
    inputs="$(get_inputs "$base")"

    # cat each file
    for f in "$main_src" "${extra_srcs[@]}"; do
        local b
        b="$(basename "$f")"
        echo ""
        echo "${PROMPT} cat ${b}"
        echo ""
        cat "$f"
    done

    echo ""
    echo "${PROMPT} gcc ${all_display}"
    gcc ${all_compile} -o a.out 2>&1

    echo "${PROMPT} ./a.out"
    run_with_pty "./a.out" "$inputs"
    echo ""
}

# ─── Internal-run mode (called by `script`) ───────────────────
if [[ "${1:-}" == "--internal-run" ]]; then

    # 1. Root directory — Main.c is multi-file
    if [[ -f "Main.c" && -f "Sender.c" && -f "Reciever.c" ]]; then
        process_multifile "Main.c" "Sender.c" "Reciever.c"
    fi

    # 2. Any other standalone .c / .cpp in root (skip multi-file components)
    for src in *.c *.cpp; do
        [[ -f "$src" ]] || continue
        # Skip the files already handled as part of multi-file build
        case "$src" in
            Main.c|Sender.c|Sender.h|Reciever.c|Reciever.h) continue ;;
        esac
        process_file "$src"
    done

    # 3. Descend into subdirectories (Excercise Questions)
    for dir in */; do
        [[ -d "$dir" ]] || continue
        pushd "$dir" > /dev/null
        PROMPT="abishekvh@Abisheks-MacBook-Air $(basename "$PWD") %"

        for src in *.c *.cpp *.java; do
            [[ -f "$src" ]] || continue
            process_file "$src"
        done

        popd > /dev/null
        PROMPT="abishekvh@Abisheks-MacBook-Air $(basename "$PWD") %"
    done

    exit 0
fi

# ─── Entry point (first invocation) ──────────────────────────
rm -f "$PRN_FILE" a.out

echo "▸ Recording to ${PRN_FILE} …"
script -q "$PRN_FILE" bash "$0" --internal-run

# Post-process: strip leftover \r bytes
if command -v sed &>/dev/null; then
    sed -i '' $'s/\r//g' "$PRN_FILE" 2>/dev/null || true
fi

echo "✔ Done!  ${PRN_FILE} is ready."
