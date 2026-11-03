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
                    data = data.replace(b'\r\n', b'\n')
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
