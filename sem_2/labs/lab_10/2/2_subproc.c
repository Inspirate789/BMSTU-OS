#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int fd[2] = {open("alphabet.txt", O_RDONLY), open("alphabet.txt", O_RDONLY)};
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
            while (read(fd[i], &c, 1) == 1)
                write(1, &c, 1);
            return 0;
        }
    
    int status;
    for (size_t i = 0; i < 2; i++)
        waitpid(pid[i], &status, 0);

    return 0;
}
