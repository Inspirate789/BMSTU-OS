#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define STR1 "How are you?"
#define STR2 "I'm ok, thank you."
#define BUF_SIZE 1024

int main(int argc, char **argv)
{
    int sockets[2];
    char buf[BUF_SIZE];
    int pid;
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0)
    {
        perror("socketpair() failed");
        return EXIT_FAILURE;
    }
    pid = fork(); // добавить проверку на -1
    {
        // ...
    }
    if (pid == 0)
    {
        close(sockets[1]);
        write(sockets[0], STR1, sizeof(STR1));
        read(sockets[0], buf, sizeof(buf));
        printf("%s\n", buf);
        close(sockets[0]);
    }
    else
    {
        close(sockets[0]);
        read(sockets[1], buf, sizeof(buf));
        printf("%s\n", buf);
        write(sockets[1], STR2, sizeof(STR2));
        close(sockets[1]);
    }
}
