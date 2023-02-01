#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    pid_t child_pid[2];

    for (size_t i = 0; i < 2; i++)
    {
        if ((child_pid[i] = fork()) == -1)
        {
            perror("Can't fork\n");
            exit(1);
        }
        else if (child_pid[i] == 0)
        {
            printf("Child: pid = %d, ppid = %d, gid = %d\n", \
                getpid(), getppid(), getpgrp());
            sleep(2);
            printf("Child: pid = %d, ppid = %d, gid = %d\n", \
                getpid(), getppid(), getpgrp());
            return 0;
        }

        printf("Parent. Children id: %d\n", child_pid[i]);
    }

    return 0;
}
