#include<stdio.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/socket.h>

int main()
{
    int s;
    struct sockaddr_in server;
    char msg[100], arr[100];

    printf("Enter String : ");
    scanf("%s", msg);

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8081);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&server, sizeof(server));

    connect(s, (struct sockaddr *)&server, sizeof(server));

    send(s, &msg, sizeof(msg), 0);
    recv(s, &arr, sizeof(arr), 0);

    printf("Recieved Successfully : %s", arr);
    return 0;
}
