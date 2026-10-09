#!/bin/bash
# ──────────────────────────────────────────────────────────────
# generate_record.sh — Auto-generates a .prn lab record for EX2
# Uses the `script` command for recording and a Python PTY
# wrapper so that user input is authentically echoed.
# ──────────────────────────────────────────────────────────────

PROMPT="abishekvh@Abisheks-MacBook-Air EX2 % "
PRN_FILE="EX2.prn"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# ── First invocation: wrap self under `script` to record ─────
if [ "$1" != "--internal-run" ]; then
    cd "$SCRIPT_DIR"
    script -q "$PRN_FILE" bash "$0" --internal-run
    # Strip \r characters injected by script's own PTY layer
    sed -i '' $'s/\r//g' "$PRN_FILE"
    exit 0
fi

# ── Input map: filename → stdin input ────────────────────────
get_input() {
    case "$1" in
        two_way_pipe.c)   echo -e "5\n1234" ;;
        process_tree.c)   echo "madam" ;;
        p.c)              echo "madam" ;;
        t.c)              echo -e "5" ;;
        *)                echo "" ;;
    esac
}

# ── PTY runner: executes ./a.out with simulated typed input ──
run_with_pty() {
    PTY_INPUT="$1" python3 << 'PYEOF'
import pty, os, sys, select, time, fcntl, termios

input_text = os.environ.get("PTY_INPUT", "")

master_fd, slave_fd = pty.openpty()
pid = os.fork()

if pid == 0:
    # ── Child: attach slave PTY as controlling terminal ──
    os.close(master_fd)
    os.setsid()
    fcntl.ioctl(slave_fd, termios.TIOCSCTTY, 0)
    os.dup2(slave_fd, 0)
    os.dup2(slave_fd, 1)
    os.dup2(slave_fd, 2)
    if slave_fd > 2:
        os.close(slave_fd)
    os.execvp("./a.out", ["./a.out"])
else:
    # ── Parent: feed input, capture output ──
    os.close(slave_fd)
    time.sleep(0.3)

    # Type each character with a small delay for realism
    if input_text:
        for ch in input_text:
            os.write(master_fd, ch.encode())
            time.sleep(0.02)
        os.write(master_fd, b'\n')
        time.sleep(0.3)

    # Read all output from the PTY master
    output = b''
    while True:
        rlist, _, _ = select.select([master_fd], [], [], 1.0)
        if rlist:
            try:
                data = os.read(master_fd, 4096)
                if not data:
                    break
                output += data
            except OSError:
                break
        else:
            # select timed out — check if child has exited
            try:
                wpid, _ = os.waitpid(pid, os.WNOHANG)
            except ChildProcessError:
                break
            if wpid != 0:
                # Child exited — drain any remaining bytes
                while True:
                    r2, _, _ = select.select([master_fd], [], [], 0.1)
                    if r2:
                        try:
                            d = os.read(master_fd, 4096)
                            if d:
                                output += d
                            else:
                                break
                        except OSError:
                            break
                    else:
                        break
                break

    # Reap child if not already reaped
    try:
        os.waitpid(pid, 0)
    except ChildProcessError:
        pass
    os.close(master_fd)

    # Clean \r\n → \n and emit to stdout (captured by script)
    text = output.decode("utf-8", errors="replace")
    text = text.replace("\r\n", "\n")
    sys.stdout.write(text)
    sys.stdout.flush()
PYEOF
}

# ── Main: iterate over .c source files ───────────────────────
for src in *.c; do
    [ -f "$src" ] || continue

    input=$(get_input "$src")

    # ── Display source code ──
    printf '%s' "$PROMPT"
    echo "cat $src"
    cat "$src"
    echo ""

    # ── Compile ──
    printf '%s' "$PROMPT"
    echo "gcc $src"
    gcc "$src" 2>&1

    # ── Execute ──
    printf '%s' "$PROMPT"
    echo "./a.out"

    if [ -n "$input" ]; then
        run_with_pty "$input"
    else
        ./a.out
    fi

    echo ""
done

# ── Cleanup ──
rm -f a.out

printf '%s' "$PROMPT"
echo "exit"
exit 0
