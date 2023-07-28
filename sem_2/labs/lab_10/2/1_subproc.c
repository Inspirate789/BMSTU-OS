#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int fd1 = open("alphabet.txt", O_RDONLY);
    int fd2 = open("alphabet.txt", O_RDONLY);
    pid_t pid;
    char c;

    if ((pid = fork()) == -1)
    {
        perror("fork\n");
        exit(1);
    }
    else if (pid == 0)
    {
        while (read(fd1, &c, 1) == 1)
            write(1, &c, 1);
        return 0;
    }
    
    usleep(10);
    while (read(fd2, &c, 1) == 1)
        write(1, &c, 1);
    
    int status;
    waitpid(pid, &status, 0);

    return 0;
}
