#ifndef NET_H
#define NET_H

#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>

#define LINE_SIZE 1024
#define CLIENT_LIMIT 64

int send_all(int fd, const void *data, size_t size);
/* 1: complete printable ASCII line; 0: clean EOF; -1: I/O error;
 * -2: malformed, oversized, or truncated line. */
int recv_line(int fd, char *line, size_t size);
int send_text(int fd, const char *format, ...);
int tcp_connect(const char *host, const char *port);
int tcp_serve(const char *port, void (*handler)(int), const char *log_path);
void log_event(int fd, const char *format, ...);
int parse_number(const char *text, int low, int high, int *value);

#endif
