#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    FILE *fs1 = fopen("1_subproc.txt", "w");
    FILE *fs2 = fopen("1_subproc.txt", "w");
    pid_t pid;

    if ((pid = fork()) == -1)
    {
        perror("fork\n");
        exit(1);
    }
    else if (pid == 0)
    {
        for (char c = 'b'; c <= 'z'; c += 2)
            fprintf(fs2, "child:  %c\n", c);
        return 0;
    }
    
    usleep(10);
    for (char c = 'a'; c <= 'z'; c += 2)
        fprintf(fs1, "parent: %c\n", c);
    
    int status;
    waitpid(pid, &status, 0);
    fclose(fs1);
    fclose(fs2);

    return 0;
}
