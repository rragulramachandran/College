#include "../common/net.h"
#include <string.h>
#include <sys/socket.h>

struct Client
{
    int fd, active, references;
    char name[32];
    pthread_mutex_t output_lock;
};
static struct Client clients[CLIENT_LIMIT];
static pthread_mutex_t clients_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t released = PTHREAD_COND_INITIALIZER;

static int deliver(struct Client *client, const char *message)
{
    pthread_mutex_lock(&client->output_lock);
    int result = send_all(client->fd, message, strlen(message));
    if (result < 0)
    {
        shutdown(client->fd, SHUT_RDWR);
    }
    pthread_mutex_unlock(&client->output_lock);
    return result;
}

static void broadcast(struct Client *sender, const char *message)
{
    struct Client *targets[CLIENT_LIMIT];
    int count = 0;
    pthread_mutex_lock(&clients_lock);
    for (int i = 0; i < CLIENT_LIMIT; i++)
    {
        if (clients[i].active && &clients[i] != sender)
        {
            targets[count++] = &clients[i];
            clients[i].references++;
        }
    }
    pthread_mutex_unlock(&clients_lock);
    for (int i = 0; i < count; i++)
    {
        deliver(targets[i], message);
        pthread_mutex_lock(&clients_lock);
        targets[i]->references--;
        pthread_cond_broadcast(&released);
        pthread_mutex_unlock(&clients_lock);
    }
}

static int valid_name(const char *name)
{
    size_t n = strlen(name);
    return n > 0 && n <= 31 &&
           strspn(name, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-") == n;
}

static void chat(int fd)
{
    struct Client *self = NULL;
    char line[LINE_SIZE], message[LINE_SIZE + 64];
    int result;
    if (send_text(fd, "OK Chat: JOIN name; MSG text; WHO; QUIT\n") < 0)
    {
        return;
    }
    while ((result = recv_line(fd, line, sizeof(line))) == 1)
    {
        log_event(fd, "request=%s", line);
        if (!strcmp(line, "QUIT"))
        {
            if (self)
            {
                deliver(self, "OK Goodbye\n");
            }
            else
            {
                send_text(fd, "OK Goodbye\n");
            }
            break;
        }
        if (!self)
        {
            if (strncmp(line, "JOIN ", 5) || !valid_name(line + 5))
            {
                if (send_text(fd, "ERR Use JOIN with a unique name\n") < 0)
                {
                    break;
                }
                continue;
            }
            pthread_mutex_lock(&clients_lock);
            int duplicate = 0;
            for (int i = 0; i < CLIENT_LIMIT; i++)
            {
                if (clients[i].active && !strcmp(clients[i].name, line + 5))
                {
                    duplicate = 1;
                }
            }
            for (int i = 0; i < CLIENT_LIMIT && !duplicate; i++)
            {
                if (clients[i].fd == -1)
                {
                    self = &clients[i];
                    /* Publish only after reserving the output lock: greeting comes first. */
                    pthread_mutex_lock(&self->output_lock);
                    self->fd = fd;
                    strcpy(self->name, line + 5);
                    self->active = 1;
                    break;
                }
            }
            pthread_mutex_unlock(&clients_lock);
            if (!self)
            {
                if (send_text(fd, "ERR Name in use or chat full\n") < 0)
                {
                    break;
                }
                continue;
            }
            int failed = send_text(fd, "OK Joined %s\n", self->name);
            pthread_mutex_unlock(&self->output_lock);
            if (failed)
            {
                break;
            }
            snprintf(message, sizeof(message), "NOTICE %s joined\n", self->name);
            broadcast(self, message);
        }
        else if (!strncmp(line, "MSG ", 4) && line[4])
        {
            snprintf(message, sizeof(message), "MESSAGE %s %s\n", self->name, line + 4);
            broadcast(self, message);
            if (deliver(self, "OK Sent\n") < 0)
            {
                break;
            }
        }
        else if (!strcmp(line, "WHO"))
        {
            char list[CLIENT_LIMIT * 40 + 32];
            size_t used = (size_t)snprintf(list, sizeof(list), "OK USERS\n");
            pthread_mutex_lock(&clients_lock);
            for (int i = 0; i < CLIENT_LIMIT; i++)
            {
                if (clients[i].active)
                {
                    used += (size_t)snprintf(list + used, sizeof(list) - used, "USER %s\n",
                                             clients[i].name);
                }
            }
            pthread_mutex_unlock(&clients_lock);
            snprintf(list + used, sizeof(list) - used, "END\n");
            if (deliver(self, list) < 0)
            {
                break;
            }
        }
        else if (deliver(self, "ERR Invalid request\n") < 0)
        {
            break;
        }
    }
    if (result < 0)
    {
        log_event(fd, "invalid/truncated request or connection timeout/error");
        if (self)
        {
            deliver(self, "ERR Invalid line or connection timeout\n");
        }
        else
        {
            send_text(fd, "ERR Invalid line or connection timeout\n");
        }
    }
    if (self)
    {
        snprintf(message, sizeof(message), "NOTICE %s left\n", self->name);
        pthread_mutex_lock(&clients_lock);
        self->active = 0;
        /* Snapshot references keep the descriptor alive until all sends finish. */
        while (self->references)
        {
            pthread_cond_wait(&released, &clients_lock);
        }
        self->fd = -1;
        pthread_mutex_unlock(&clients_lock);
        broadcast(NULL, message);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }
    for (int i = 0; i < CLIENT_LIMIT; i++)
    {
        clients[i].fd = -1;
        pthread_mutex_init(&clients[i].output_lock, NULL);
    }
    return tcp_serve(argv[1], chat, "chat.log");
}
