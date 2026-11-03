#define _POSIX_C_SOURCE 200809L
#include "net.h"
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

/* Interactive line-protocol client shared by examination, hotel, and chat. */
int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <server> <port>\n", argv[0]);
        return 1;
    }
    signal(SIGPIPE, SIG_IGN);
    int fd = tcp_connect(argv[1], argv[2]);
    if (fd < 0)
    {
        perror("connect");
        return 1;
    }
    struct pollfd events[2] =
    {
        {
            STDIN_FILENO, POLLIN, 0
        },
        {
            fd, POLLIN, 0
        }
    };
    int status = 0;
    for (;;)
    {
        if (poll(events, 2, -1) < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror("poll");
            status = 1;
            break;
        }
        char buffer[4096];
        if (events[1].revents & (POLLIN | POLLHUP | POLLERR))
        {
            ssize_t n = recv(fd, buffer, sizeof(buffer), 0);
            if (n <= 0)
            {
                status = n < 0;
                break;
            }
            fwrite(buffer, 1, (size_t)n, stdout);
            fflush(stdout);
        }
        if (events[0].revents & (POLLIN | POLLHUP))
        {
            ssize_t n = read(STDIN_FILENO, buffer, sizeof(buffer));
            if (n <= 0)
            {
                shutdown(fd, SHUT_WR);
                events[0].fd = -1;
            }
            else if (send_all(fd, buffer, (size_t)n) < 0)
            {
                perror("send");
                status = 1;
                break;
            }
        }
    }
    close(fd);
    return status;
}
