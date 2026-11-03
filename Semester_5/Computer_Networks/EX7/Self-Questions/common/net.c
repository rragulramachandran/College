#define _POSIX_C_SOURCE 200809L
#include "net.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

static pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t count_lock = PTHREAD_MUTEX_INITIALIZER;
static FILE *activity_log;
static int client_count;

int parse_number(const char *text, int low, int high, int *value)
{
    char *end;
    errno = 0;
    long n = strtol(text, &end, 10);
    if (errno || !*text || *end || n < low || n > high)
    {
        return 0;
    }
    *value = (int)n;
    return 1;
}

int send_all(int fd, const void *data, size_t size)
{
    const char *bytes = data;
    while (size)
    {
        ssize_t n = send(fd, bytes, size, 0);
        if (n < 0 && errno == EINTR)
        {
            continue;
        }
        if (n <= 0)
        {
            return -1;
        }
        bytes += n;
        size -= (size_t)n;
    }
    return 0;
}

int send_text(int fd, const char *format, ...)
{
    char text[4096];
    va_list args;
    va_start(args, format);
    int n = vsnprintf(text, sizeof(text), format, args);
    va_end(args);
    if (n < 0 || (size_t)n >= sizeof(text))
    {
        return -1;
    }
    return send_all(fd, text, (size_t)n);
}

int recv_line(int fd, char *line, size_t size)
{
    size_t used = 0;
    int cr = 0;
    for (;;)
    {
        unsigned char c;
        ssize_t n = recv(fd, &c, 1, 0);
        if (n < 0 && errno == EINTR)
        {
            continue;
        }
        if (n <= 0)
        {
            return n == 0 ? (used || cr ? -2 : 0) : -1;
        }
        if (c == '\n')
        {
            line[used] = '\0';
            return 1;
        }
        if (cr)
        {
            return -2;
        }
        if (c == '\r')
        {
            cr = 1;
            continue;
        }
        if (c < 32 || c > 126 || used + 1 >= size)
        {
            return -2;
        }
        line[used++] = (char)c;
    }
}

void log_event(int fd, const char *format, ...)
{
    pthread_mutex_lock(&log_lock);
    time_t now = time(NULL);
    struct tm date;
    char stamp[32];
    localtime_r(&now, &date);
    strftime(stamp, sizeof(stamp), "%Y-%m-%dT%H:%M:%S%z", &date);
    FILE *out = activity_log ? activity_log : stderr;
    fprintf(out, "%s client=%d ", stamp, fd);
    va_list args;
    va_start(args, format);
    vfprintf(out, format, args);
    va_end(args);
    fputc('\n', out);
    if (fflush(out) == EOF)
    {
        perror("activity log");
    }
    pthread_mutex_unlock(&log_lock);
}

int tcp_connect(const char *host, const char *port)
{
    struct addrinfo hints =
    {
        0
    };
    struct addrinfo *addresses;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int error = getaddrinfo(host, port, &hints, &addresses);
    if (error)
    {
        fprintf(stderr, "Address: %s\n", gai_strerror(error));
        return -1;
    }
    int fd = -1;
    for (struct addrinfo *a = addresses; a; a = a->ai_next)
    {
        fd = socket(a->ai_family, a->ai_socktype, a->ai_protocol);
        if (fd >= 0 && connect(fd, a->ai_addr, a->ai_addrlen) == 0)
        {
            break;
        }
        if (fd >= 0)
        {
            close(fd);
        }
        fd = -1;
    }
    freeaddrinfo(addresses);
    return fd;
}

struct Worker
{
    int fd;
    void (*handler)(int);
};

static void *worker(void *arg)
{
    struct Worker work = *(struct Worker *)arg;
    free(arg);
    work.handler(work.fd);
    log_event(work.fd, "disconnected");
    close(work.fd);
    pthread_mutex_lock(&count_lock);
    client_count--;
    pthread_mutex_unlock(&count_lock);
    return NULL;
}

int tcp_serve(const char *port, void (*handler)(int), const char *log_path)
{
    int number;
    if (!parse_number(port, 1, 65535, &number))
    {
        fprintf(stderr, "Invalid port\n");
        return 1;
    }
    int log_fd = open(log_path, O_WRONLY | O_CREAT | O_APPEND, 0600);
    activity_log = log_fd < 0 ? NULL : fdopen(log_fd, "a");
    if (!activity_log)
    {
        perror(log_path);
        return 1;
    }
    signal(SIGPIPE, SIG_IGN);
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    struct sockaddr_in address =
    {
        0
    };
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons((unsigned short)number);
    if (listener < 0 || setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0 ||
        bind(listener, (struct sockaddr *)&address, sizeof(address)) < 0 ||
        listen(listener, 64) < 0)
    {
        perror("listen");
        if (listener >= 0)
        {
            close(listener);
        }
        fclose(activity_log);
        return 1;
    }
    printf("Listening on port %d\n", number);
    fflush(stdout);
    log_event(listener, "server started port=%d", number);
    for (;;)
    {
        struct sockaddr_in peer;
        socklen_t length = sizeof(peer);
        int fd = accept(listener, (struct sockaddr *)&peer, &length);
        if (fd < 0)
        {
            if (errno != EINTR)
            {
                perror("accept");
            }
            continue;
        }
        /* A stalled reader cannot hold a broadcast or database lock indefinitely. */
        struct timeval timeout =
        {
            5, 0
        };
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
        timeout.tv_sec = 300;
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &peer.sin_addr, ip, sizeof(ip));
        log_event(fd, "connected peer=%s:%u", ip, ntohs(peer.sin_port));
        pthread_mutex_lock(&count_lock);
        int full = client_count == CLIENT_LIMIT;
        if (!full)
        {
            client_count++;
        }
        pthread_mutex_unlock(&count_lock);
        struct Worker *work = full ? NULL : malloc(sizeof(*work));
        pthread_t thread;
        int error = 0;
        if (work)
        {
            work->fd = fd;
            work->handler = handler;
            error = pthread_create(&thread, NULL, worker, work);
        }
        if (!work || error)
        {
            send_text(fd, "ERR Server busy\n");
            log_event(fd, "rejected: server busy");
            close(fd);
            free(work);
            if (!full)
            {
                pthread_mutex_lock(&count_lock);
                client_count--;
                pthread_mutex_unlock(&count_lock);
            }
            continue;
        }
        pthread_detach(thread);
    }
}
