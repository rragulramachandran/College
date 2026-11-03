#include "../common/net.h"
#include <string.h>

struct Student
{
    const char *username;
    const char *password;
    int active;
};

static struct Student students[] =
{
    {
        "student1", "exam1", 0
    },
    {
        "student2", "exam2", 0
    },
    {
        "student3", "exam3", 0
    },
    {
        "student4", "exam4", 0
    }
};
static pthread_mutex_t session_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t question_lock = PTHREAD_MUTEX_INITIALIZER;

struct Question
{
    const char *text;
    char correct;
};
static const struct Question questions[] =
{
    {
        "Which protocol provides reliable delivery? A=UDP B=TCP C=IP D=ARP", 'B'
    },
    {
        "Which call accepts a TCP connection? A=bind B=listen C=accept D=socket", 'C'
    },
    {
        "Which primitive protects shared data? A=mutex B=port C=packet D=address", 'A'
    }
};
#define QUESTION_COUNT (sizeof(questions) / sizeof(questions[0]))
#define STUDENT_COUNT (sizeof(students) / sizeof(students[0]))

static void examine(int fd)
{
    char line[LINE_SIZE], answers[QUESTION_COUNT];
    memset(answers, 0, sizeof(answers));
    int student = -1, finished = 0, score = 0, result;
    if (send_text(fd, "OK Examination: LOGIN username password; QUESTIONS; ANSWER id A|B|C|D; "
                      "FINISH; SCORE; QUIT\n") < 0)
    {
        return;
    }
    while ((result = recv_line(fd, line, sizeof(line))) == 1)
    {
        char user[32], password[32], extra;
        int id;
        char number[32];
        char answer;
        /* Never put authentication passwords in the activity log. */
        log_event(fd, "user=%s request=%s", student < 0 ? "anonymous" : students[student].username,
                  strncmp(line, "LOGIN", 5) == 0 ? "LOGIN [credentials redacted]" : line);
        if (strcmp(line, "QUIT") == 0)
        {
            send_text(fd, "OK Goodbye\n");
            break;
        }
        if (!strncmp(line, "LOGIN ", 6) &&
            sscanf(line, "LOGIN %31s %31s %c", user, password, &extra) == 2)
        {
            if (student >= 0)
            {
                if (send_text(fd, "ERR Already authenticated\n") < 0)
                {
                    break;
                }
                continue;
            }
            pthread_mutex_lock(&session_lock);
            for (size_t i = 0; i < STUDENT_COUNT; i++)
            {
                if (!strcmp(user, students[i].username) &&
                    !strcmp(password, students[i].password) && !students[i].active)
                {
                    students[i].active = 1;
                    student = (int)i;
                    break;
                }
            }
            pthread_mutex_unlock(&session_lock);
            log_event(fd, "authentication %s",
                      student >= 0 ? "successful" : "failed or already active");
            if (send_text(fd, student >= 0 ? "OK Authenticated\n"
                                           : "ERR Invalid credentials or active session\n") < 0)
            {
                break;
            }
        }
        else if (student < 0)
        {
            if (send_text(fd, "ERR Authenticate first\n") < 0)
            {
                break;
            }
        }
        else if (!strcmp(line, "QUESTIONS"))
        {
            /* Copy the database under lock; never hold this lock during network I/O. */
            struct Question snapshot[QUESTION_COUNT];
            pthread_mutex_lock(&question_lock);
            memcpy(snapshot, questions, sizeof(snapshot));
            pthread_mutex_unlock(&question_lock);
            int failed = send_text(fd, "OK QUESTIONS %zu\n", QUESTION_COUNT);
            for (size_t i = 0; i < QUESTION_COUNT && !failed; i++)
            {
                failed = send_text(fd, "QUESTION %zu %s\n", i + 1, snapshot[i].text);
            }
            if (failed || send_text(fd, "END\n") < 0)
            {
                break;
            }
        }
        else if (!strncmp(line, "ANSWER ", 7) &&
                 sscanf(line, "ANSWER %31s %c %c", number, &answer, &extra) == 2 &&
                 parse_number(number, 1, (int)QUESTION_COUNT, &id) && answer >= 'A' &&
                 answer <= 'D')
        {
            if (!finished)
            {
                answers[id - 1] = answer;
            }
            if (send_text(fd, finished ? "ERR Examination already finished\n"
                                       : "OK Answer saved\n") < 0)
            {
                break;
            }
        }
        else if (!strcmp(line, "FINISH") || !strcmp(line, "SCORE"))
        {
            if (!finished && !strcmp(line, "FINISH"))
            {
                pthread_mutex_lock(&question_lock);
                for (size_t i = 0; i < QUESTION_COUNT; i++)
                {
                    score += answers[i] == questions[i].correct;
                }
                pthread_mutex_unlock(&question_lock);
                finished = 1;
                log_event(fd, "user=%s evaluated score=%d/%zu", students[student].username, score,
                          QUESTION_COUNT);
            }
            if ((finished ? send_text(fd, "OK SCORE %d/%zu\n", score, QUESTION_COUNT)
                          : send_text(fd, "ERR Finish the examination first\n")) < 0)
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
    if (student >= 0)
    {
        pthread_mutex_lock(&session_lock);
        students[student].active = 0;
        pthread_mutex_unlock(&session_lock);
        log_event(fd, "user=%s session closed", students[student].username);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }
    return tcp_serve(argv[1], examine, "examination.log");
}
