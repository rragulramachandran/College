#!/bin/bash

# Configuration
EX_NAME="EX3"

if [ "$1" != "--internal-run" ]; then
    echo "Starting script to generate ${EX_NAME}.prn..."
    script -q "${EX_NAME}.prn" bash "$0" --internal-run
    echo "Done! The output has been saved to ${EX_NAME}.prn"
    exit 0
fi

# Create Python wrapper script once
cat << 'EOF' > /tmp/pty_wrapper.py
import pty
import os
import sys

# use unicode_escape to convert literal \n strings to actual newlines
input_data = sys.argv[1].encode('utf-8').decode('unicode_escape').encode('utf-8')
cmd = sys.argv[2:]

pid, fd = pty.fork()
if pid == 0:
    os.execvp(cmd[0], cmd)
else:
    if input_data:
        os.write(fd, input_data)
        
    while True:
        try:
            data = os.read(fd, 1024)
            if not data:
                break
            sys.stdout.buffer.write(data.replace(b'\r\n', b'\n'))
            sys.stdout.buffer.flush()
        except OSError:
            break
    _, status = os.waitpid(pid, 0)
    sys.exit(os.WIFEXITED(status) and os.WEXITSTATUS(status) or 1)
EOF

simulate_prompt() {
    local dir_name=$(basename "$PWD")
    echo -n "abishekvh@Abisheks-MacBook-Air ${dir_name} % "
}

process_dir() {
    local dir="$1"
    pushd "$dir" > /dev/null

    shopt -s nullglob
    local c_files=(*.c)
    local cpp_files=(*.cpp)
    local h_files=(*.h)
    shopt -u nullglob
    
    if [ ${#c_files[@]} -eq 0 ] && [ ${#cpp_files[@]} -eq 0 ]; then
        popd > /dev/null
        return
    fi
    
    for f in "${c_files[@]}" "${cpp_files[@]}" "${h_files[@]}"; do
        simulate_prompt
        echo "cat $f"
        cat "$f"
    done

    local compiler=""
    local compile_cmd=""
    
    if [ ${#cpp_files[@]} -gt 0 ]; then
        compiler="g++"
        compile_cmd="$compiler *.cpp -o a.out"
    elif [ ${#c_files[@]} -gt 0 ]; then
        compiler="gcc"
        compile_cmd="$compiler *.c -o a.out"
    fi
    
    if [ -n "$compiler" ]; then
        simulate_prompt
        echo "$compile_cmd"
        eval "$compile_cmd"
        
        if [ -f "a.out" ]; then
            simulate_prompt
            echo "./a.out"
            
            local input=""
            case "$(basename "$PWD")" in
                "2DParity")
                    input="2\n2\n1\n0\n0\n1\n1\n1\n"
                    ;;
                "HammingCodes")
                    input="4\n1\n0\n1\n1\n3\n"
                    ;;
                *)
                    input=""
                    ;;
            esac
            
            python3 /tmp/pty_wrapper.py "$input" ./a.out
            rm -f a.out
        fi
    fi
    
    popd > /dev/null
}

process_dir "."

find . -mindepth 1 -type d | sort | while read -r d; do
    if [[ "$d" != *".git"* ]] && [[ "$d" != *".gemini"* ]]; then
        process_dir "$d"
    fi
done

rm -f /tmp/pty_wrapper.py
