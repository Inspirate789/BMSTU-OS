#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    FILE *fs[2] = {fopen("2_subproc.txt", "w"), fopen("2_subproc.txt", "w")};
    pid_t pid[2];

    for (size_t i = 0; i < 2; i++)
        if ((pid[i] = fork()) == -1)
        {
            perror("Can't fork\n");
            exit(1);
        }
        else if (pid[i] == 0)
        {
            for (char c = 'b'; c <= 'z'; c += 2)
                fprintf(fs[i], "child %d: %c\n", i, c);
            return 0;
        }
    
    int status;
    for (size_t i = 0; i < 2; i++)
        waitpid(pid[i], &status, 0);
    fclose(fs[0]);
    fclose(fs[1]);

    return 0;
}
