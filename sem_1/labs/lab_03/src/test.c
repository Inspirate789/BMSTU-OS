#include <stdio.h>
#include <unistd.h>

int main(void)
{
    pid_t childpid;

    if ((childpid = fork()) == -1)
    {
        perror("Can't fork\n");
        exit(1);
    }
    else if (childpid == 0)
    {
        printf("Child: pid = %d, ppid = %d, gid = %d\n", \
            getpid(), getppid(), getgid());
    }
    else
    {
        printf("Parent: pid = %d, childpid = %d, gid = %d\n", \
            getpid(), childpid, getgid());
    }

    return 0;
}
