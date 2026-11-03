#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define LOG_FILE "server_activity.log"
#define EOF_MARKER "__EOF__"

const char *VALID_USER = "admin";
const char *VALID_PASS = "password123";

void log_activity(const char *username, const char *ip, int port,
                  const char *action) {
  FILE *log_fp = fopen(LOG_FILE, "a");
  if (!log_fp) {
    perror("[-] Error opening log file");
    return;
  }
  time_t now = time(NULL);
  char *time_str = ctime(&now);
  time_str[strlen(time_str) - 1] = '\0';

  fprintf(log_fp, "[%s] User: %s | Client IP: %s:%d | Action: %s\n", time_str,
          username, ip, port, action);
  fclose(log_fp);
  printf("[SERVER LOG] User: %s | %s:%d | Action: %s\n", username, ip, port,
         action);
}

void *handle_client(void *socket_desc) {
  int client_sock = *(int *)socket_desc;
  free(socket_desc);

  struct sockaddr_in client_addr;
  socklen_t addr_size = sizeof(client_addr);
  getpeername(client_sock, (struct sockaddr *)&client_addr, &addr_size);

  char client_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
  int client_port = ntohs(client_addr.sin_port);

  char buffer[BUFFER_SIZE];
  char username[50] = "Unauthenticated";
  char password[50] = {0};

  send(client_sock, "AUTH_REQ", 8, 0);

  memset(buffer, 0, BUFFER_SIZE);
  int read_size = recv(client_sock, buffer, BUFFER_SIZE, 0);
  if (read_size <= 0) {
    close(client_sock);
    return NULL;
  }

  char *colon = strchr(buffer, ':');
  if (colon) {
    *colon = '\0';
    strncpy(username, buffer, sizeof(username) - 1);
    strncpy(password, colon + 1, sizeof(password) - 1);

    if (strcmp(username, VALID_USER) == 0 &&
        strcmp(password, VALID_PASS) == 0) {
      send(client_sock, "AUTH_SUCCESS", 12, 0);
      log_activity(username, client_ip, client_port, "Successfully Logged In");
    } else {
      send(client_sock, "AUTH_FAIL", 9, 0);
      log_activity(username, client_ip, client_port, "Failed Login Attempt");
      close(client_sock);
      return NULL;
    }
  } else {
    send(client_sock, "AUTH_FAIL", 9, 0);
    close(client_sock);
    return NULL;
  }

  while (1) {
    memset(buffer, 0, BUFFER_SIZE);
    read_size = recv(client_sock, buffer, BUFFER_SIZE, 0);
    if (read_size <= 0)
      break;

    if (strncmp(buffer, "UPLOAD:", 7) == 0) {
      char *filename = buffer + 7;
      send(client_sock, "READY", 5, 0);

      char server_filename[100];
      snprintf(server_filename, sizeof(server_filename), "server_%s", filename);
      FILE *fp = fopen(server_filename, "wb");

      while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recv(client_sock, buffer, BUFFER_SIZE, 0);
        if (bytes <= 0)
          break;

        char *eof_pos = strstr(buffer, EOF_MARKER);
        if (eof_pos != NULL) {
          fwrite(buffer, 1, eof_pos - buffer, fp);
          break;
        }
        fwrite(buffer, 1, bytes, fp);
      }
      fclose(fp);
      send(client_sock, "UPLOAD_SUCCESS", 14, 0);

      char log_msg[150];
      snprintf(log_msg, sizeof(log_msg), "Uploaded file %s", server_filename);
      log_activity(username, client_ip, client_port, log_msg);

    } else if (strncmp(buffer, "DOWNLOAD:", 9) == 0) {
      char *filename = buffer + 9;
      FILE *fp = fopen(filename, "rb");

      if (fp) {
        send(client_sock, "FILE_FOUND", 10, 0);
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_sock, buffer, BUFFER_SIZE, 0);

        int bytes_read;
        while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
          send(client_sock, buffer, bytes_read, 0);
        }
        fclose(fp);
        usleep(100000);
        send(client_sock, EOF_MARKER, strlen(EOF_MARKER), 0);

        char log_msg[150];
        snprintf(log_msg, sizeof(log_msg), "Downloaded file %s", filename);
        log_activity(username, client_ip, client_port, log_msg);
      } else {
        send(client_sock, "FILE_NOT_FOUND", 14, 0);
        log_activity(username, client_ip, client_port,
                     "Requested a non-existent file path");
      }

    } else if (strcmp(buffer, "DATE_TIME") == 0) {
      time_t now = time(NULL);
      char *time_str = ctime(&now);
      send(client_sock, time_str, strlen(time_str), 0);
      log_activity(username, client_ip, client_port,
                   "Requested Date and Time Services");

    } else if (strcmp(buffer, "SYS_INFO") == 0) {
      char *sys_info = "OS Platform: POSIX Linux Core | Arch: Multi-Threaded "
                       "TCP Server Module";
      send(client_sock, sys_info, strlen(sys_info), 0);
      log_activity(username, client_ip, client_port,
                   "Requested Server Diagnostics");

    } else if (strcmp(buffer, "TERMINATE") == 0) {
      send(client_sock, "GOODBYE", 7, 0);
      log_activity(username, client_ip, client_port,
                   "Terminated session gracefully");
      break;
    } else {
      send(client_sock, "INVALID_CMD", 11, 0);
      log_activity(username, client_ip, client_port,
                   "Sent unknown invalid syntax request");
    }
  }

  close(client_sock);
  return NULL;
}

int main() {
  int server_fd, *new_sock;
  struct sockaddr_in address;
  int opt = 1;

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("[-] Socket creation failed");
    exit(EXIT_FAILURE);
  }
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("[-] Bind failed");
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 5) < 0) {
    perror("[-] Listen failed");
    exit(EXIT_FAILURE);
  }

  printf("[SERVER STARTUP] TCP Engine Listening on loopback port %d...\n",
         PORT);

  while (1) {
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    int client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
    if (client_fd < 0)
      continue;

    pthread_t thread_id;
    new_sock = malloc(sizeof(int));
    *new_sock = client_fd;

    pthread_create(&thread_id, NULL, handle_client, (void *)new_sock);
    pthread_detach(thread_id);
  }
  close(server_fd);
  return 0;
}
