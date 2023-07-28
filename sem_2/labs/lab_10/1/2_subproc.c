#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int fd = open("alphabet.txt", O_RDONLY);
    FILE *fs[2] = {fdopen(fd, "r"), fdopen(fd, "r")};
    char buff[2][20];
    setvbuf(fs[0], buff[0], _IOFBF, 20);
    setvbuf(fs[1], buff[1], _IOFBF, 20);
    pid_t pid[2];
    char c;

    for (size_t i = 0; i < 2; i++)
        if ((pid[i] = fork()) == -1)
        {
            perror("Can't fork\n");
            exit(1);
        }
        else if (pid[i] == 0)
        {
            while (fscanf(fs[i], "%c", &c) == 1)
                fprintf(stdout, "child %d:  %c\n", i, c);
            return 0;
        }
    
    int status;
    for (size_t i = 0; i < 2; i++)
        waitpid(pid[i], &status, 0);

    return 0;
}
