#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define PORT 7004
#define TITLE "College Library Server"
#define DATA_FILE "library.dat"
#define LOG_FILE "library.log"
#define HELP                                                                                       \
    "list\nsearch|title_or_author_text\navailability|book_id\nissue|book_id|borrower_"             \
    "name\nreturn|issue_id\nissued|borrower_name\nhelp\nquit\n"
struct Book
{
    char title[70], author[60], category[40];
    int available;
};
struct Issue
{
    int book, active;
    char borrower[60];
};
typedef struct
{
    struct Book books[4];
    struct Issue issues[200];
    int count;
} Database;

static Database db;
static char output[32768];

static void reply(const char *format, ...)
{
    size_t used = strlen(output);
    va_list args;
    va_start(args, format);
    vsnprintf(output + used, sizeof(output) - used, format, args);
    va_end(args);
}

static int number(const char *text, int maximum)
{
    for (const char *p = text; *p; p++)
    {
        if (*p < '0' || *p > '9')
        {
            return 0;
        }
    }
    char *end;
    errno = 0;
    long value = strtol(text, &end, 10);
    return errno || !*text || *end || value < 1 || value > maximum ? 0 : (int)value;
}

static void log_activity(const char *peer, const char *action)
{
    FILE *file = fopen(LOG_FILE, "a");
    if (!file)
    {
        perror(LOG_FILE);
        return;
    }
    time_t now = time(NULL);
    char stamp[32];
    strftime(stamp, sizeof(stamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    fprintf(file, "[%s] %s: %s\n", stamp, peer, action);
    fclose(file);
}

/* Replace the database only after the complete new snapshot is written. */
static int save_database(void)
{
    FILE *file = fopen(DATA_FILE ".tmp", "wb");
    if (!file)
    {
        return 0;
    }
    int ok = fwrite(&db, sizeof(db), 1, file) == 1;
    if (fflush(file) || fsync(fileno(file)))
    {
        ok = 0;
    }
    if (fclose(file))
    {
        ok = 0;
    }
    if (ok && rename(DATA_FILE ".tmp", DATA_FILE) == 0)
    {
        return 1;
    }
    remove(DATA_FILE ".tmp");
    return 0;
}

static int send_text(int client, const char *text)
{
    size_t left = strlen(text);
    while (left)
    {
        ssize_t n = send(client, text, left, 0);
        if (n < 0 && errno == EINTR)
        {
            continue;
        }
        if (n <= 0)
        {
            return 0;
        }
        text += n;
        left -= (size_t)n;
    }
    return 1;
}

/* TCP is a byte stream: collect one newline-terminated command at a time. */
static int read_line(int client, char *line, size_t size)
{
    size_t used = 0;
    int invalid = 0;
    for (;;)
    {
        unsigned char ch;
        ssize_t n = recv(client, &ch, 1, 0);
        if (n < 0 && errno == EINTR)
        {
            continue;
        }
        if (n <= 0)
        {
            return 0;
        }
        if (ch == '\n')
        {
            line[used] = '\0';
            return invalid ? -1 : 1;
        }
        if (ch == '\r')
        {
            continue;
        }
        if (ch < 32 || ch == 127 || used + 1 >= size)
        {
            invalid = 1;
        }
        else
        {
            line[used++] = (char)ch;
        }
    }
}

/* Preserve empty fields so malformed requests cannot shift arguments. */
static int split(char *line, char **fields)
{
    int count = 1;
    fields[0] = line;
    for (char *p = line; *p; p++)
    {
        if (*p == '|')
        {
            if (count == 8)
            {
                return 0;
            }
            *p = '\0';
            fields[count++] = p + 1;
        }
    }
    for (int i = 0; i < count; i++)
    {
        while (*fields[i] == ' ')
        {
            fields[i]++;
        }
        char *end = fields[i] + strlen(fields[i]);
        while (end > fields[i] && end[-1] == ' ')
        {
            *--end = '\0';
        }
        if (!*fields[i])
        {
            return 0;
        }
    }
    return count;
}

static void initialize(void);
static int handle(int count, char **fields);

int main(void)
{
    FILE *file = fopen(DATA_FILE, "rb");
    if (file)
    {
        int ok = fread(&db, sizeof(db), 1, file) == 1 && fgetc(file) == EOF;
        fclose(file);
        if (!ok)
        {
            fprintf(stderr, "Invalid database file: %s\n", DATA_FILE);
            return 1;
        }
    }
    else
    {
        if (errno != ENOENT)
        {
            perror(DATA_FILE);
            return 1;
        }
        initialize();
        if (!save_database())
        {
            perror("Create database");
            return 1;
        }
    }
    signal(SIGPIPE, SIG_IGN);
    int server = socket(AF_INET, SOCK_STREAM, 0), reuse = 1;
    if (server < 0)
    {
        perror("socket");
        return 1;
    }
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
    if (bind(server, (struct sockaddr *)&address, sizeof(address)) < 0 || listen(server, 8) < 0)
    {
        perror("bind/listen");
        close(server);
        return 1;
    }
    printf("%s listening on 127.0.0.1:%d\n", TITLE, PORT);
    for (;;)
    {
        struct sockaddr_in address;
        socklen_t length = sizeof(address);
        int client = accept(server, (struct sockaddr *)&address, &length);
        if (client < 0)
        {
            if (errno != EINTR)
            {
                perror("accept");
            }
            continue;
        }
        char peer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &address.sin_addr, peer, sizeof(peer));
        log_activity(peer, "CONNECTED");
        if (send_text(client, TITLE "\n" HELP "> "))
        {
            char line[512], *fields[8];
            int result;
            /* Finish this entire session before calling accept again. */
            while ((result = read_line(client, line, sizeof(line))) != 0)
            {
                output[0] = '\0';
                if (result < 0)
                {
                    reply("ERROR: Command too long or contains control characters.\n");
                    log_activity(peer, "INVALID COMMAND");
                }
                else
                {
                    log_activity(peer, line);
                    int count = split(line, fields);
                    if (count == 1 && strcmp(fields[0], "quit") == 0)
                    {
                        send_text(client, "Goodbye!\n");
                        break;
                    }
                    Database before = db;
                    if (count == 1 && strcmp(fields[0], "help") == 0)
                    {
                        reply("%s", HELP);
                    }
                    else if (handle(count, fields) && !save_database())
                    {
                        db = before;
                        output[0] = '\0';
                        reply("ERROR: Database save failed; transaction rolled back.\n");
                    }
                }
                log_activity(peer, output);
                reply("> ");
                if (!send_text(client, output))
                {
                    break;
                }
            }
        }
        close(client);
        log_activity(peer, "DISCONNECTED");
    }
}

static void initialize(void)
{
    db.books[0] = (struct Book){"Computer Networks", "Andrew S. Tanenbaum", "Networking", 1};
    db.books[1] =
        (struct Book){"The C Programming Language", "Kernighan and Ritchie", "Programming", 1};
    db.books[2] =
        (struct Book){"Operating System Concepts", "Abraham Silberschatz", "Operating Systems", 1};
    db.books[3] = (struct Book){"Database System Concepts", "Henry F. Korth", "Databases", 1};
}

static int contains(const char *text, const char *query)
{
    for (; *text; text++)
    {
        if (!strncasecmp(text, query, strlen(query)))
        {
            return 1;
        }
    }
    return 0;
}

static void show_book(int i)
{
    struct Book *b = &db.books[i];
    reply("%d | %s | %s | %s | %s\n", i + 1, b->title, b->author, b->category,
          b->available ? "AVAILABLE" : "ISSUED");
}

static int handle(int count, char **f)
{
    if ((count == 1 && !strcmp(f[0], "list")) || (count == 2 && !strcmp(f[0], "search")))
    {
        int found = 0;
        for (int i = 0; i < 4; i++)
        {
            if (count == 1 || contains(db.books[i].title, f[1]) ||
                contains(db.books[i].author, f[1]))
            {
                show_book(i);
                found = 1;
            }
        }
        if (!found)
        {
            reply("No matching books.\n");
        }
    }
    else if (count == 2 && !strcmp(f[0], "availability"))
    {
        int id = number(f[1], 4);
        if (id)
        {
            show_book(id - 1);
        }
        else
        {
            reply("ERROR: Book not found.\n");
        }
    }
    else if (count == 3 && !strcmp(f[0], "issue"))
    {
        int id = number(f[1], 4);
        if (!id || strlen(f[2]) >= 60)
        {
            reply("ERROR: Invalid book or borrower name.\n");
        }
        else if (!db.books[id - 1].available || db.count == 200)
        {
            reply("ERROR: Book already issued or issue database full.\n");
        }
        else
        {
            struct Issue *issue = &db.issues[db.count++];
            *issue = (struct Issue){.book = id, .active = 1};
            strcpy(issue->borrower, f[2]);
            db.books[id - 1].available = 0;
            reply("ISSUED: Issue %d | %s | %s\n", db.count, db.books[id - 1].title,
                  issue->borrower);
            return 1;
        }
    }
    else if (count == 2 && !strcmp(f[0], "return"))
    {
        int id = number(f[1], db.count);
        if (!id || !db.issues[id - 1].active)
        {
            reply("ERROR: Issue not found or book already returned.\n");
        }
        else
        {
            db.issues[id - 1].active = 0;
            db.books[db.issues[id - 1].book - 1].available = 1;
            reply("RETURNED: Issue %d; book is available again.\n", id);
            return 1;
        }
    }
    else if (count == 2 && !strcmp(f[0], "issued"))
    {
        int found = 0;
        for (int i = 0; i < db.count; i++)
        {
            struct Issue *issue = &db.issues[i];
            if (!strcasecmp(issue->borrower, f[1]))
            {
                reply("Issue %d | Book %d | %s | %s | %s\n", i + 1, issue->book,
                      db.books[issue->book - 1].title, issue->borrower,
                      issue->active ? "ISSUED" : "RETURNED");
                found = 1;
            }
        }
        if (!found)
        {
            reply("No issue records for this borrower.\n");
        }
    }
    else
    {
        reply("ERROR: Invalid command or arguments. Type help.\n");
    }
    return 0;
}
