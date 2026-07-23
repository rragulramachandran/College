#!/usr/bin/env bash
# ──────────────────────────────────────────────────────────────
# generate_record.sh — EX1 Lab Record Generator
# Compiles & runs every .c / .cpp file, records output to EX1.prn
# using the macOS `script` command + a Python PTY wrapper so that
# stdin echo appears exactly as if a human typed it.
# ──────────────────────────────────────────────────────────────

set -euo pipefail

EX_NAME="EX1"
PRN_FILE="${EX_NAME}.prn"
PROMPT="abishekvh@Abisheks-MacBook-Air $(basename "$PWD") %"

# ─── Input Map ────────────────────────────────────────────────
# Returns the newline-separated list of inputs for a given file.
get_inputs() {
    local file="$1"
    case "$file" in

        BISYNC.c)
            # Enter Header : HDR01
            # Enter Body   : HelloDLEWorld
            cat <<'EOF'
HDR01
HelloDLEWorld
EOF
            ;;

        DDCMP.c)
            # Do you have data to send? (y/n): y
            # Enter data to send: HelloDDCMP
            # Do you have data to send? (y/n): n
            cat <<'EOF'
y
HelloDDCMP
n
EOF
            ;;

        LayeringandFraming.c)
            # Menu option 5 → Network Layers (displays table then asks URLs)
            # Enter Source URL      : www.google.com
            # Enter Destination URL : www.youtube.com
            cat <<'EOF'
5
www.google.com
www.youtube.com
EOF
            ;;

        # ── Fallback: program needs no input ──
        *)
            echo ""
            ;;
    esac
}

# ─── Python PTY runner ────────────────────────────────────────
# Runs a command inside a pseudo-terminal, feeding each line of
# $INPUTS with a small delay so the program can print its prompt
# before we send the next keystroke.  \r\n → \n cleanup is done
# in-line so the .prn file stays pristine.
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
    """Read everything available on fd within *timeout* seconds."""
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
            end = time.time() + timeout   # reset on activity
        else:
            break
    return buf

# Drain initial output (prompts, banners, etc.)
sys.stdout.buffer.write(read_all(master_fd).replace(b"\r\n", b"\n"))
sys.stdout.buffer.flush()

for line in inputs:
    os.write(master_fd, (line + "\n").encode())
    time.sleep(0.15)                       # let the program react
    data = read_all(master_fd, timeout=0.5)
    sys.stdout.buffer.write(data.replace(b"\r\n", b"\n"))
    sys.stdout.buffer.flush()

# Wait for process & drain anything left
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

# ─── Internal-run mode (called by `script`) ───────────────────
if [[ "${1:-}" == "--internal-run" ]]; then

    process_file() {
        local src="$1"
        local ext="${src##*.}"
        local compiler

        if [[ "$ext" == "c" ]]; then
            compiler="gcc"
        else
            compiler="g++"
        fi

        local inputs
        inputs="$(get_inputs "$(basename "$src")")"

        echo ""
        echo "${PROMPT} cat $(basename "$src")"
        echo ""
        cat "$src"

        echo ""
        echo "${PROMPT} ${compiler} $(basename "$src")"
        ${compiler} "$src" -o a.out 2>&1

        echo "${PROMPT} ./a.out"
        run_with_pty "./a.out" "$inputs"
        echo ""
    }

    # 1. Process .c / .cpp in the current directory
    for src in *.c *.cpp; do
        [[ -f "$src" ]] || continue
        # Skip header-only / non-main files
        [[ "$src" == *.h ]] && continue
        process_file "$src"
    done

    # 2. Descend into subdirectories (e.g. "Excercise Questions")
    for dir in */; do
        [[ -d "$dir" ]] || continue
        pushd "$dir" > /dev/null
        for src in *.c *.cpp; do
            [[ -f "$src" ]] || continue
            [[ "$src" == *.h ]] && continue
            process_file "$src"
        done
        popd > /dev/null
    done

    exit 0
fi

# ─── Entry point (first invocation) ──────────────────────────
# Remove stale output
rm -f "$PRN_FILE" a.out

echo "▸ Recording to ${PRN_FILE} …"
script -q "$PRN_FILE" bash "$0" --internal-run

# Post-process: strip any leftover \r bytes
if command -v sed &>/dev/null; then
    sed -i '' $'s/\r//g' "$PRN_FILE" 2>/dev/null || true
fi

echo "✔ Done!  ${PRN_FILE} is ready."
