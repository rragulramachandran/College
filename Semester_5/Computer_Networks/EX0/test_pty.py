import pty, os, sys, time

pid, fd = pty.fork()
if pid == 0:
    os.execv('./test_expect', ['./test_expect'])
else:
    inputs = sys.argv[1].split('\\n')
    
    # Read output in a non-blocking way to ensure we see the prompt first
    import select
    
    def read_all():
        while True:
            r, _, _ = select.select([fd], [], [], 0.1)
            if fd in r:
                try:
                    data = os.read(fd, 1024)
                    if not data: return False
                    sys.stdout.buffer.write(data)
                    sys.stdout.buffer.flush()
                except OSError:
                    return False
            else:
                break
        return True
        
    read_all()
    
    for inp in inputs:
        os.write(fd, (inp + '\n').encode())
        read_all()
        
    while read_all():
        pass
