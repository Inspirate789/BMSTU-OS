#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define REQUESTS_COUNT 10

int main(void)
{
    int fd;
    char buf[128];
    int option = 1;

    if ((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket() failed");
        exit(1);
    }

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)
    {
        perror("setsockopt() failed");
        exit(1);
    }

    struct sockaddr server_name, client_name;
    server_name.sa_family = AF_UNIX;
    strcpy(server_name.sa_data, "./test_sock");
    int sock_len = sizeof(server_name);

    if (bind(fd, &server_name, sock_len) == -1)
    {
        perror("bind() failed");
        exit(1);
    }

    printf("Server is waiting for messages\n");

    for (int i = 0; i < REQUESTS_COUNT; ++i)
    {
        if (recvfrom(fd, buf, sizeof(buf), 0, &client_name, &sock_len) == -1)
        {
            perror("recvfrom() failed");
            exit(1);
        }

        printf("Server received message from client %s\n", buf);
    }

    close(fd);

    return 0;
}
