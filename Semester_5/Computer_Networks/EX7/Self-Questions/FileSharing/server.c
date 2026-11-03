#define _DARWIN_C_SOURCE
#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200809L
#include "../common/net.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int root;

static int safe_name(const char *name)
{
    size_t n = strlen(name);
    return n > 0 && n <= 255 && strcmp(name, ".") && strcmp(name, "..") &&
           strspn(name, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._-") == n;
}

static int open_file(const char *name, struct stat *info)
{
    if (!safe_name(name))
    {
        return -1;
    }
    int fd = openat(root, name, O_RDONLY | O_NOFOLLOW | O_NONBLOCK);
    if (fd < 0)
    {
        return -1;
    }
    if (fstat(fd, info) < 0 || !S_ISREG(info->st_mode) || info->st_size < 0)
    {
        close(fd);
        return -1;
    }
    return fd;
}

static int list_files(int fd)
{
    /* Open a fresh directory description so concurrent lists have separate offsets. */
    int directory_fd = openat(root, ".", O_RDONLY | O_DIRECTORY);
    DIR *directory = directory_fd < 0 ? NULL : fdopendir(directory_fd);
    if (!directory)
    {
        if (directory_fd >= 0)
        {
            close(directory_fd);
        }
        return send_text(fd, "ERR Cannot list files\n");
    }
    int failed = send_text(fd, "OK FILES\n");
    struct dirent *entry;
    while (!failed)
    {
        errno = 0;
        entry = readdir(directory);
        if (!entry)
        {
            if (errno)
            {
                failed = -1;
            }
            break;
        }
        struct stat info;
        int file = open_file(entry->d_name, &info);
        if (file >= 0)
        {
            close(file);
            failed = send_text(fd, "FILE %s\n", entry->d_name);
        }
    }
    closedir(directory);
    return failed ? -1 : send_text(fd, "END\n");
}

static int download(int fd, const char *name)
{
    struct stat info;
    int file = open_file(name, &info);
    if (file < 0)
    {
        return send_text(fd, "ERR File unavailable or invalid name\n");
    }
    uintmax_t remaining = (uintmax_t)info.st_size;
    int failed = send_text(fd, "DATA %ju\n", remaining);
    char buffer[16384];
    while (remaining && !failed)
    {
        size_t amount = remaining < sizeof(buffer) ? (size_t)remaining : sizeof(buffer);
        ssize_t n = read(file, buffer, amount);
        if (n < 0 && errno == EINTR)
        {
            continue;
        }
        if (n <= 0)
        {
            failed = -1;
            break;
        }
        failed = send_all(fd, buffer, (size_t)n);
        remaining -= (size_t)n;
    }
    close(file);
    log_event(fd, "download=%s status=%s", name, failed ? "interrupted" : "complete");
    /* Never insert an error line inside the promised binary payload. */
    return failed;
}

static void share(int fd)
{
    char line[LINE_SIZE];
    int result;
    if (send_text(fd, "OK File sharing: LIST; GET filename; QUIT\n") < 0)
    {
        return;
    }
    while ((result = recv_line(fd, line, sizeof(line))) == 1)
    {
        log_event(fd, "request=%s", line);
        if (!strcmp(line, "QUIT"))
        {
            send_text(fd, "OK Goodbye\n");
            break;
        }
        if (!strcmp(line, "LIST"))
        {
            if (list_files(fd) < 0)
            {
                break;
            }
        }
        else if (!strncmp(line, "GET ", 4))
        {
            if (download(fd, line + 4) < 0)
            {
                break;
            }
        }
        else if (send_text(fd, "ERR Invalid request\n") < 0)
        {
            break;
        }
    }
    if (result < 0)
    {
        log_event(fd, "invalid/truncated request or connection timeout/error");
        send_text(fd, "ERR Invalid line or connection timeout\n");
    }
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <port> <shared-directory>\n", argv[0]);
        return 1;
    }
    root = open(argv[2], O_RDONLY | O_DIRECTORY);
    if (root < 0)
    {
        perror("shared directory");
        return 1;
    }
    return tcp_serve(argv[1], share, "files.log");
}
