#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int fd = open("alphabet.txt", O_RDONLY);
    FILE *fs1 = fdopen(fd, "r"), *fs2 = fdopen(fd, "r");
    char buff1[20], buff2[20];
    setvbuf(fs1, buff1, _IOFBF, 20);
    setvbuf(fs2, buff2, _IOFBF, 20);
    pid_t pid;
    char c;

    if ((pid = fork()) == -1)
    {
        perror("fork\n");
        exit(1);
    }
    else if (pid == 0)
    {
        while (fscanf(fs2, "%c", &c) == 1)
            fprintf(stdout, "child:  %c\n", c);
        return 0;
    }
    
    usleep(10);
    while (fscanf(fs1, "%c", &c) == 1)
        fprintf(stdout, "parent: %c\n", c);
    
    int status;
    waitpid(pid, &status, 0);

    return 0;
}
