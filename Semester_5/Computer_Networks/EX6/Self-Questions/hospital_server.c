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

#define PORT 7002
#define TITLE "Hospital Appointment Server"
#define DATA_FILE "hospital.dat"
#define LOG_FILE "hospital.log"
#define HELP                                                                                       \
    "doctors\nregister|patient_name|age\navailability|doctor_id|YYYY-MM-DD\nbook|patient_id|"      \
    "doctor_id|YYYY-MM-DD|slot_id\ncancel|appointment_id\nhistory|patient_id\nhelp\nquit\n"
struct Doctor
{
    char name[60], specialty[40];
    char slots[4][20];
};
struct Patient
{
    char name[60];
    int age;
};
struct Appointment
{
    int patient, doctor, slot, active;
    char date[11];
};
typedef struct
{
    struct Doctor doctors[3];
    struct Patient patients[100];
    struct Appointment appointments[200];
    int patient_count, appointment_count;
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
    db.doctors[0] = (struct Doctor){"Dr. Kumar",
                                    "General Medicine",
                                    {"09:00-09:30", "09:30-10:00", "10:00-10:30", "10:30-11:00"}};
    db.doctors[1] = (struct Doctor){
        "Dr. Priya", "Cardiology", {"14:00-14:30", "14:30-15:00", "15:00-15:30", "15:30-16:00"}};
    db.doctors[2] = (struct Doctor){
        "Dr. Arun", "Dermatology", {"16:00-16:30", "16:30-17:00", "17:00-17:30", "17:30-18:00"}};
}

static int valid_date(const char *date)
{
    if (strlen(date) != 10 || date[4] != '-' || date[7] != '-')
    {
        return 0;
    }
    for (int i = 0; i < 10; i++)
    {
        if (i != 4 && i != 7 && (date[i] < '0' || date[i] > '9'))
        {
            return 0;
        }
    }
    int year, month, day;
    if (sscanf(date, "%4d-%2d-%2d", &year, &month, &day) != 3 || year < 1900 || month < 1 ||
        month > 12)
    {
        return 0;
    }
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    days[1] += year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
    return day >= 1 && day <= days[month - 1];
}

static int occupied(int doctor, const char *date, int slot)
{
    for (int i = 0; i < db.appointment_count; i++)
    {
        struct Appointment *a = &db.appointments[i];
        if (a->active && a->doctor == doctor && a->slot == slot && !strcmp(a->date, date))
        {
            return 1;
        }
    }
    return 0;
}

static int handle(int count, char **f)
{
    if (count == 1 && !strcmp(f[0], "doctors"))
    {
        for (int i = 0; i < 3; i++)
        {
            reply("%d | %s | %s | Daily: %s to %s\n", i + 1, db.doctors[i].name,
                  db.doctors[i].specialty, db.doctors[i].slots[0], db.doctors[i].slots[3]);
        }
    }
    else if (count == 3 && !strcmp(f[0], "register"))
    {
        int age = number(f[2], 120);
        if (!age || strlen(f[1]) >= 60 || db.patient_count == 100)
        {
            reply("ERROR: Invalid age/name or patient database full.\n");
        }
        else
        {
            struct Patient *p = &db.patients[db.patient_count++];
            strcpy(p->name, f[1]);
            p->age = age;
            reply("REGISTERED: Patient %d | %s | Age: %d\n", db.patient_count, p->name, age);
            return 1;
        }
    }
    else if (count == 3 && !strcmp(f[0], "availability"))
    {
        int doctor = number(f[1], 3);
        if (!doctor || !valid_date(f[2]))
        {
            reply("ERROR: Invalid doctor or date.\n");
        }
        else
        {
            for (int i = 1; i <= 4; i++)
            {
                reply("Slot %d | %s | %s | %s\n", i, f[2], db.doctors[doctor - 1].slots[i - 1],
                      occupied(doctor, f[2], i) ? "BOOKED" : "AVAILABLE");
            }
        }
    }
    else if (count == 5 && !strcmp(f[0], "book"))
    {
        int patient = number(f[1], db.patient_count), doctor = number(f[2], 3);
        int slot = number(f[4], 4);
        if (!patient || !doctor || !slot || !valid_date(f[3]))
        {
            reply("ERROR: Invalid patient, doctor, date, or slot.\n");
        }
        else if (db.appointment_count == 200 || occupied(doctor, f[3], slot))
        {
            reply("ERROR: Slot already booked or appointment database full.\n");
        }
        else
        {
            struct Appointment *a = &db.appointments[db.appointment_count++];
            *a = (struct Appointment){
                .patient = patient, .doctor = doctor, .slot = slot, .active = 1};
            strcpy(a->date, f[3]);
            reply("CONFIRMED: Appointment %d | %s | %s | %s | %s\n", db.appointment_count,
                  db.patients[patient - 1].name, db.doctors[doctor - 1].name, a->date,
                  db.doctors[doctor - 1].slots[slot - 1]);
            return 1;
        }
    }
    else if (count == 2 && !strcmp(f[0], "cancel"))
    {
        int id = number(f[1], db.appointment_count);
        if (!id || !db.appointments[id - 1].active)
        {
            reply("ERROR: Appointment not found or already cancelled.\n");
        }
        else
        {
            db.appointments[id - 1].active = 0;
            reply("CANCELLED: Appointment %d; slot is available again.\n", id);
            return 1;
        }
    }
    else if (count == 2 && !strcmp(f[0], "history"))
    {
        int patient = number(f[1], db.patient_count), found = 0;
        if (!patient)
        {
            reply("ERROR: Patient not found.\n");
            return 0;
        }
        reply("Patient %d | %s | Age: %d\n", patient, db.patients[patient - 1].name,
              db.patients[patient - 1].age);
        for (int i = 0; i < db.appointment_count; i++)
        {
            struct Appointment *a = &db.appointments[i];
            if (a->patient == patient)
            {
                reply("Appointment %d | %s | %s | %s | %s\n", i + 1, db.doctors[a->doctor - 1].name,
                      a->date, db.doctors[a->doctor - 1].slots[a->slot - 1],
                      a->active ? "CONFIRMED" : "CANCELLED");
                found = 1;
            }
        }
        if (!found)
        {
            reply("No appointments.\n");
        }
    }
    else
    {
        reply("ERROR: Invalid command or arguments. Type help.\n");
    }
    return 0;
}
