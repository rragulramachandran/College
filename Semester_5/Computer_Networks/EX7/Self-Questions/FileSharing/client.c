#define _POSIX_C_SOURCE 200809L
#include "../common/net.h"
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static int save_file(int fd, const char *path, uintmax_t remaining)
{
    int output = open(path, O_WRONLY | O_CREAT | O_EXCL, 0600);
    if (output < 0)
    {
        perror(path);
        return 1;
    }
    int failed = 0;
    char buffer[16384];
    while (remaining)
    {
        size_t amount = remaining < sizeof(buffer) ? (size_t)remaining : sizeof(buffer);
        ssize_t n = recv(fd, buffer, amount, 0);
        if (n < 0 && errno == EINTR)
        {
            continue;
        }
        if (n <= 0)
        {
            failed = 1;
            break;
        }
        size_t used = 0;
        while (used < (size_t)n)
        {
            ssize_t written = write(output, buffer + used, (size_t)n - used);
            if (written < 0 && errno == EINTR)
            {
                continue;
            }
            if (written <= 0)
            {
                failed = 1;
                break;
            }
            used += (size_t)written;
        }
        if (failed)
        {
            break;
        }
        remaining -= (size_t)n;
    }
    if (fsync(output) < 0)
    {
        failed = 1;
    }
    if (close(output) < 0)
    {
        failed = 1;
    }
    if (failed)
    {
        unlink(path);
        fprintf(stderr, "Transfer failed; partial file removed\n");
    }
    else
    {
        printf("Saved %s\n", path);
    }
    return failed;
}

int main(int argc, char **argv)
{
    int listing = argc == 4 && !strcmp(argv[3], "LIST");
    if (!listing && !(argc == 6 && !strcmp(argv[3], "GET")))
    {
        fprintf(stderr,
                "Usage: %s <server> <port> LIST\n"
                "       %s <server> <port> GET <remote-name> <local-path>\n",
                argv[0], argv[0]);
        return 1;
    }
    signal(SIGPIPE, SIG_IGN);
    int fd = tcp_connect(argv[1], argv[2]);
    if (fd < 0)
    {
        perror("connect");
        return 1;
    }
    char line[LINE_SIZE];
    int status = 1;
    if (recv_line(fd, line, sizeof(line)) != 1 || strncmp(line, "OK ", 3))
    {
        goto done;
    }
    if (listing)
    {
        if (send_text(fd, "LIST\n") < 0 || recv_line(fd, line, sizeof(line)) != 1 ||
            strcmp(line, "OK FILES"))
        {
            goto done;
        }
        while (recv_line(fd, line, sizeof(line)) == 1)
        {
            if (!strcmp(line, "END"))
            {
                status = 0;
                break;
            }
            if (strncmp(line, "FILE ", 5))
            {
                break;
            }
            puts(line + 5);
        }
    }
    else
    {
        if (strpbrk(argv[4], "\r\n") || send_text(fd, "GET %s\n", argv[4]) < 0 ||
            recv_line(fd, line, sizeof(line)) != 1)
        {
            goto done;
        }
        if (strncmp(line, "DATA ", 5))
        {
            fprintf(stderr, "%s\n", line);
            goto done;
        }
        char *end;
        errno = 0;
        uintmax_t size = strtoumax(line + 5, &end, 10);
        if (errno || !line[5] || *end || strspn(line + 5, "0123456789") != strlen(line + 5))
        {
            goto done;
        }
        status = save_file(fd, argv[5], size);
    }
done:
    close(fd);
    if (status)
    {
        fprintf(stderr, "Request failed\n");
    }
    return status;
}
