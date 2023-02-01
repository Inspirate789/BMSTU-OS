#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    pid_t child_pid[2];
    char *commands[2] = {"./app1.exe", "./app2.exe"};
    char *args1[] = {"./app1.exe", "a", "in11.txt", \
                     "in12.txt", "out1.txt", NULL};
    char *args2[] = {"./app2.exe", "in2.txt", "title", "BruceAlmighty", NULL};
    char **args[] = {args1, args2};

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
            
            if (execvp(commands[i], args[i])  == -1)
            {
                perror("Can't exec\n");
                exit(1);
            }

            return 0;
        }

        printf("Parent. Children id: %d\n", child_pid[i]);
    }

    for (size_t i = 0; i < 2; i++)
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
