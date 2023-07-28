#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

// Useful links:
// https://www.ibm.com/docs/en/aix/7.1?topic=examples-socketpair-communication-example
// https://www.gnu.org/software/libc/manual/html_node/Socket-Pairs.html
// https://www.gnu.org/software/libc/manual/html_node/Pipes-and-FIFOs.html

int main(void)
{
    pid_t child_pid[3];
    char *messages[6] = {"aaa", 
                         "bbbbb",
                         "ccccccc",
                         "ddddddddd",
                         "eeeeeeeeeee",
                         "fffffffffffff"};
    int sockets[2];
    char buf[64];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1)
    {
        perror("socketpair() failed");
        exit(1);
    }

    printf("Sockets file descriptors: %d %d\n", sockets[0], sockets[1]);

    for (size_t i = 0; i < 3; i++)
    {
        if ((child_pid[i] = fork()) == -1)
        {
            perror("Can't fork\n");
            exit(1);
        }
        else if (child_pid[i] == 0)
        {
            // sleep(1);
            close(sockets[1]);
            write(sockets[0], messages[i + 3], strlen(messages[i + 3]) + 1);
            printf("Child (pid: %d)    send: %s\n", getpid(), messages[i + 3]);
            sleep(1);
            read(sockets[0], buf, strlen(messages[i]) + 1);
            printf("Child (pid: %d) receive: %s\n", getpid(), buf);
            close(sockets[0]);

            return 0;
        }
        else
        {
            sleep(1);
            read(sockets[1], buf, strlen(messages[i + 3]) + 1);
            printf("Parent            receive: %s\n", buf);

            write(sockets[1], messages[i], strlen(messages[i]) + 1);
            printf("Parent               send: %s\n", messages[i]);
        }
    }

    close(sockets[0]);
    close(sockets[1]);

    for (size_t i = 0; i < 3; i++)
    {
        int status;
        waitpid(child_pid[i], &status, 0);

        if (WIFEXITED(status))
        {
            printf("Child (pid: %d) exited with code %d\n", \
                child_pid[i], WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("Child (pid: %d) received signal %d\n", \
                child_pid[i], WTERMSIG(status));
        }
        else if (WIFSTOPPED(status))
        {
            printf("Child (pid: %d) received signal %d\n", \
                child_pid[i], WSTOPSIG(status));
        }
    }

    return 0;
}
