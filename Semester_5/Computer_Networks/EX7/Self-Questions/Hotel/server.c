#define _DARWIN_C_SOURCE
#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200809L
#include "../common/net.h"
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>

#define ROOM_COUNT 10
struct Reservation
{
    char name[32];
    char token[33];
};
static struct Reservation rooms[ROOM_COUNT];
static pthread_mutex_t booking_lock = PTHREAD_MUTEX_INITIALIZER;
static int journal, storage_failed;

static int valid_name(const char *name)
{
    size_t n = strlen(name);
    if (!n || n > 31)
    {
        return 0;
    }
    for (size_t i = 0; i < n; i++)
    {
        char c = name[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
              c == '_' || c == '-'))
        {
            return 0;
        }
    }
    return 1;
}

static int valid_token(const char *token)
{
    return strlen(token) == 32 && strspn(token, "0123456789abcdef") == 32;
}

static int make_token(char token[33])
{
    unsigned char bytes[16];
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0)
    {
        return -1;
    }
    size_t used = 0;
    while (used < sizeof(bytes))
    {
        ssize_t n = read(fd, bytes + used, sizeof(bytes) - used);
        if (n < 0 && errno == EINTR)
        {
            continue;
        }
        if (n <= 0)
        {
            close(fd);
            return -1;
        }
        used += (size_t)n;
    }
    close(fd);
    for (size_t i = 0; i < sizeof(bytes); i++)
    {
        sprintf(token + 2 * i, "%02x", bytes[i]);
    }
    return 0;
}

/* Caller holds booking_lock. Commit to disk before changing the in-memory state.
 * A failed/partial append is rolled back; subsequent writes are disabled. */
static int commit(const char *record)
{
    if (storage_failed)
    {
        return -1;
    }
    off_t start = lseek(journal, 0, SEEK_END);
    if (start < 0)
    {
        storage_failed = 1;
        return -1;
    }
    size_t remaining = strlen(record);
    const char *p = record;
    while (remaining)
    {
        ssize_t n = write(journal, p, remaining);
        if (n < 0 && errno == EINTR)
        {
            continue;
        }
        if (n <= 0)
        {
            break;
        }
        p += n;
        remaining -= (size_t)n;
    }
    if (remaining || fsync(journal) < 0)
    {
        storage_failed = 1;
        if (ftruncate(journal, start) < 0 || fsync(journal) < 0)
        {
            log_event(-1, "journal rollback failed; manual storage recovery required");
        }
        return -1;
    }
    return 0;
}

static int load_journal(const char *path)
{
    journal = open(path, O_RDWR | O_CREAT | O_APPEND, 0600);
    if (journal < 0 || flock(journal, LOCK_EX | LOCK_NB) < 0)
    {
        perror("reservation journal (must not be used by another server)");
        return -1;
    }
    FILE *input = fdopen(dup(journal), "r");
    if (!input)
    {
        perror("journal reader");
        return -1;
    }
    char line[256], token[33], name[32], number[32], extra;
    int room;
    while (fgets(line, sizeof(line), input))
    {
        if (!strchr(line, '\n'))
        {
            goto invalid;
        }
        if (!strncmp(line, "BOOK ", 5) &&
            sscanf(line, "BOOK %31s %32s %31s %c", number, token, name, &extra) == 3 &&
            parse_number(number, 1, ROOM_COUNT, &room) && valid_token(token) && valid_name(name) &&
            !rooms[room - 1].token[0])
        {
            strcpy(rooms[room - 1].name, name);
            strcpy(rooms[room - 1].token, token);
        }
        else if (!strncmp(line, "CANCEL ", 7) &&
                 sscanf(line, "CANCEL %31s %32s %c", number, token, &extra) == 2 &&
                 parse_number(number, 1, ROOM_COUNT, &room) && valid_token(token) &&
                 !strcmp(rooms[room - 1].token, token))
        {
            memset(&rooms[room - 1], 0, sizeof(rooms[0]));
        }
        else
        {
            goto invalid;
        }
    }
    if (ferror(input))
    {
        goto invalid;
    }
    fclose(input);
    return 0;
invalid:
    fprintf(stderr,
            "Invalid or incomplete reservation journal; refusing to discard transactions\n");
    fclose(input);
    return -1;
}

static void reserve(int fd)
{
    char line[LINE_SIZE];
    int result;
    if (send_text(fd, "OK Hotel: AVAILABLE; BOOK room name; CANCEL token; DETAILS token; QUIT\n") <
        0)
    {
        return;
    }
    while ((result = recv_line(fd, line, sizeof(line))) == 1)
    {
        /* The private log is an audit trail; booking tokens are bearer credentials. */
        log_event(fd, "request=%s", line);
        if (!strcmp(line, "QUIT"))
        {
            send_text(fd, "OK Goodbye\n");
            break;
        }
        char response[2048], token[33], name[32], number[32], extra;
        int room;
        pthread_mutex_lock(&booking_lock);
        if (!strcmp(line, "AVAILABLE"))
        {
            size_t used = (size_t)snprintf(response, sizeof(response), "OK AVAILABLE\n");
            for (int i = 0; i < ROOM_COUNT; i++)
            {
                used += (size_t)snprintf(response + used, sizeof(response) - used, "ROOM %d %s\n",
                                         i + 1, rooms[i].token[0] ? "BOOKED" : "FREE");
            }
            snprintf(response + used, sizeof(response) - used, "END\n");
        }
        else if (!strncmp(line, "BOOK ", 5) &&
                 sscanf(line, "BOOK %31s %31s %c", number, name, &extra) == 2 &&
                 parse_number(number, 1, ROOM_COUNT, &room) && valid_name(name))
        {
            if (rooms[room - 1].token[0])
            {
                strcpy(response, "ERR Room already booked\n");
            }
            else if (make_token(token) < 0)
            {
                strcpy(response, "ERR Cannot generate reservation token\n");
            }
            else
            {
                char record[128];
                snprintf(record, sizeof(record), "BOOK %d %s %s\n", room, token, name);
                if (commit(record) < 0)
                {
                    strcpy(response, "ERR Storage failure; booking not committed\n");
                }
                else
                {
                    strcpy(rooms[room - 1].name, name);
                    strcpy(rooms[room - 1].token, token);
                    snprintf(response, sizeof(response), "OK BOOKED %d TOKEN %s\n", room, token);
                }
            }
        }
        else if (((!strncmp(line, "CANCEL ", 7) &&
                   sscanf(line, "CANCEL %32s %c", token, &extra) == 1) ||
                  (!strncmp(line, "DETAILS ", 8) &&
                   sscanf(line, "DETAILS %32s %c", token, &extra) == 1)) &&
                 valid_token(token))
        {
            room = -1;
            for (int i = 0; i < ROOM_COUNT; i++)
            {
                if (!strcmp(rooms[i].token, token))
                {
                    room = i;
                    break;
                }
            }
            strcpy(response, "ERR Reservation not found\n");
            if (room >= 0)
            {
                if (!strncmp(line, "DETAILS ", 8))
                {
                    snprintf(response, sizeof(response), "OK BOOKING %d %s\n", room + 1,
                             rooms[room].name);
                }
                else
                {
                    char record[128];
                    snprintf(record, sizeof(record), "CANCEL %d %s\n", room + 1, token);
                    if (commit(record) < 0)
                    {
                        strcpy(response, "ERR Storage failure; cancellation not committed\n");
                    }
                    else
                    {
                        memset(&rooms[room], 0, sizeof(rooms[room]));
                        strcpy(response, "OK Cancelled\n");
                    }
                }
            }
        }
        else
        {
            strcpy(response, "ERR Invalid request\n");
        }
        pthread_mutex_unlock(&booking_lock);
        log_event(fd, "result=%.*s", (int)strcspn(response, "\n"), response);
        if (send_all(fd, response, strlen(response)) < 0)
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
        fprintf(stderr, "Usage: %s <port> <journal-file>\n", argv[0]);
        return 1;
    }
    if (load_journal(argv[2]) < 0)
    {
        return 1;
    }
    return tcp_serve(argv[1], reserve, "hotel.log");
}
