#!/bin/bash

if [ "$1" == "--internal-run" ]; then
    
    run_program() {
        local file="$1"
        local input=""
        
        # Determine the correct input based on the file name
        case "$file" in
            "LayeringAndFraming.c")
                input="5\nwww.mepcoeng.ac.in\nwww.google.com"
                ;;
            "3407_Substring_Pattern_Matching.cpp")
                input="leetcode\nee*e"
                ;;
            "338_Counting_Bits.cpp")
                input="2"
                ;;
            "49_Group_Anagrams.cpp")
                # The C++ code expects: int n; then n strings
                input="6\neat\ntea\ntan\nate\nnat\nbat"
                ;;
            "String_Processing_CodeChef.cpp")
                input="4\n100\nCCCCCCCCCCCCCC\n400\nCDCDCDCDCDCDCD\n30\nDDCCNNDDDCCNND\n1\nNNDNNDDDNNDNDN"
                ;;
            *)
                input="1" # Default dummy input
                ;;
        esac

        local prompt="abishekvh@Abisheks-MacBook-Air $(basename "$PWD") %"
        
        echo "$prompt cat $file"
        cat "$file"
        
        if [[ "$file" == *.c ]]; then
            echo "$prompt gcc $file"
            gcc "$file"
        else
            echo "$prompt g++ $file"
            g++ "$file"
        fi
        
        if [ $? -eq 0 ] && [ -x ./a.out ]; then
            echo "$prompt ./a.out"
            python3 -c '
import pty, os, sys, select

pid, fd = pty.fork()
if pid == 0:
    os.execv("./a.out", ["./a.out"])
else:
    inputs = sys.argv[1].split("\\n")
    
    def read_all():
        while True:
            r, _, _ = select.select([fd], [], [], 0.05)
            if fd in r:
                try:
                    data = os.read(fd, 1024)
                    if not data: return False
                    data = data.replace(b"\r\n", b"\n")
                    sys.stdout.buffer.write(data)
                    sys.stdout.buffer.flush()
                except OSError:
                    return False
            else:
                break
        return True
        
    read_all()
    
    for inp in inputs:
        os.write(fd, (inp + "\n").encode())
        read_all()
        
    while read_all():
        pass
' "$input"
        else
            echo "$prompt Error: Compilation failed for $file"
        fi
    }

    # Process .c files in the current working directory
    for file in *.c; do
        if [ -f "$file" ]; then
            run_program "$file"
        fi
    done
    
    # Process .cpp files in the "Excercise Questions" directory
    if [ -d "Excercise Questions" ]; then
        cd "Excercise Questions" || exit 1
        for file in *.cpp; do
            if [ -f "$file" ]; then
                run_program "$file"
            fi
        done
        cd ..
    else
        echo -e "\n--- Directory 'Excercise Questions' not found ---"
    fi
    
    exit 0
fi

# Main entry point
script EX0.prn bash "$0" --internal-run
