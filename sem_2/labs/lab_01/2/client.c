#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        perror("Incorrect arguments count");
        exit(1);
    }

    int fd;

    if ((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket() failed");
        exit(1);
    }

    struct sockaddr server_name;
    server_name.sa_family = AF_UNIX;
    strcpy(server_name.sa_data, "./test_sock");
    int sock_len = sizeof(server_name);

    char buf[128];
    snprintf(buf, 128, "%s (pid %d)", argv[1], getpid());

    if (sendto(fd, buf, strlen(buf) + 1, 0, &server_name, sock_len) == -1)
    {
        perror("sendto() failed");
        exit(1);
    }

    close(fd);

    return 0;
}