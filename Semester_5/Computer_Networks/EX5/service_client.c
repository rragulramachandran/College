#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define EOF_MARKER "__EOF__"

int main() {
  int sock = 0;
  struct sockaddr_in serv_addr;
  char buffer[BUFFER_SIZE] = {0};

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\n [-] Socket creation error \n");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    printf("\n[-] Invalid address or target configuration unassigned \n");
    return -1;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("\n[-] Connection Refused. Ensure server is executing first.\n");
    return -1;
  }

  recv(sock, buffer, BUFFER_SIZE, 0);
  if (strcmp(buffer, "AUTH_REQ") == 0) {
    char user[50], pass[50], payload[105];
    printf("\n============================================\n");
    printf("       SECURE TCP SERVICE LOGIN SYSTEM      \n");
    printf("============================================\n");
    printf("Enter Username: ");
    scanf("%49s", user);
    printf("Enter Password: ");
    scanf("%49s", pass);

    snprintf(payload, sizeof(payload), "%s:%s", user, pass);
    send(sock, payload, strlen(payload), 0);

    memset(buffer, 0, BUFFER_SIZE);
    recv(sock, buffer, BUFFER_SIZE, 0);

    if (strcmp(buffer, "AUTH_SUCCESS") == 0) {
      struct sockaddr_in local_addr;
      socklen_t addr_len = sizeof(local_addr);
      getsockname(sock, (struct sockaddr *)&local_addr, &addr_len);

      printf("\n[+] LOGIN SUCCESSFUL!");
      printf("\n[+] Assigned Endpoint Metrics -> Client IP: %s | Port: %d\n",
             inet_ntoa(local_addr.sin_addr), ntohs(local_addr.sin_port));
    } else {
      printf("\n[-] AUTHENTICATION FAILED! Closing socket connection "
             "descriptor.\n");
      close(sock);
      return -1;
    }
  }

  int choice;
  while (1) {
    printf("\n----------- SERVICE DASHBOARD MENU -----------\n");
    printf("1. Upload File\n");
    printf("2. Download File\n");
    printf("3. Query Server Date/Time\n");
    printf("4. Query Server System Specs\n");
    printf("5. Terminate Active Session Securely\n");
    printf("Choose an Option (1-5): ");
    if (scanf("%d", &choice) != 1) {
      while (getchar() != '\n')
        ;
      choice = -1;
    }

    if (choice == 1) {
      char filename[100];
      printf("Enter target file name on disk to read & upload: ");
      scanf("%99s", filename);

      FILE *fp = fopen(filename, "rb");
      if (!fp) {
        printf("[-] Local file error: target file name not found locally.\n");
        continue;
      }

      snprintf(buffer, sizeof(buffer), "UPLOAD:%s", filename);
      send(sock, buffer, strlen(buffer), 0);

      memset(buffer, 0, BUFFER_SIZE);
      recv(sock, buffer, BUFFER_SIZE, 0);

      if (strcmp(buffer, "READY") == 0) {
        int bytes;
        while ((bytes = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
          send(sock, buffer, bytes, 0);
        }
        fclose(fp);
        usleep(100000);
        send(sock, EOF_MARKER, strlen(EOF_MARKER), 0);

        memset(buffer, 0, BUFFER_SIZE);
        recv(sock, buffer, BUFFER_SIZE, 0);
        printf("[REPLY] Status: %s\n", buffer);
      }
    } else if (choice == 2) {
      char filename[100];
      printf("Enter complete file target path on the remote server: ");
      scanf("%99s", filename);

      snprintf(buffer, sizeof(buffer), "DOWNLOAD:%s", filename);
      send(sock, buffer, strlen(buffer), 0);

      memset(buffer, 0, BUFFER_SIZE);
      recv(sock, buffer, BUFFER_SIZE, 0);

      if (strcmp(buffer, "FILE_FOUND") == 0) {
        send(sock, "READY", 5, 0);
        FILE *fp = fopen("client_downloaded.txt", "wb");
        while (1) {
          memset(buffer, 0, BUFFER_SIZE);
          int bytes = recv(sock, buffer, BUFFER_SIZE, 0);
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
        printf("[SUCCESS] Saved locally as 'client_downloaded.txt'\n");
      } else {
        printf("[REPLY] Remote file query failed: File not found.\n");
      }
    } else if (choice == 3) {
      send(sock, "DATE_TIME", 9, 0);
      memset(buffer, 0, BUFFER_SIZE);
      recv(sock, buffer, BUFFER_SIZE, 0);
      printf("[REPLY] Server Clock Data: %s", buffer);
    } else if (choice == 4) {
      send(sock, "SYS_INFO", 8, 0);
      memset(buffer, 0, BUFFER_SIZE);
      recv(sock, buffer, BUFFER_SIZE, 0);
      printf("[REPLY] Remote Specs Details: %s\n", buffer);
    } else if (choice == 5) {
      send(sock, "TERMINATE", 9, 0);
      memset(buffer, 0, BUFFER_SIZE);
      recv(sock, buffer, BUFFER_SIZE, 0);
      printf("[REPLY] Session Context Closed: %s.\n", buffer);
      break;
    } else {
      send(sock, "UNSUPPORTED_TOKEN_FALLBACK", 26, 0);
      memset(buffer, 0, BUFFER_SIZE);
      recv(sock, buffer, BUFFER_SIZE, 0);
      printf("[REPLY Error Handler Fallback]: %s\n", buffer);
    }
  }
  close(sock);
  return 0;
}
