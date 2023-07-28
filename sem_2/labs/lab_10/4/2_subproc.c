#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int fd[2] = {open("2_subproc.txt", O_CREAT | O_WRONLY), 
                 open("2_subproc.txt", O_CREAT | O_WRONLY | O_APPEND)};
    pid_t pid[2];

    for (size_t i = 0; i < 2; i++)
        if ((pid[i] = fork()) == -1)
        {
            perror("Can't fork\n");
            exit(1);
        }
        else if (pid[i] == 0)
        {
            for (char c = 'a' + 13 * i; c <= 'm' + 13 * i; c++)
                write(fd[i], &c, 1);
            return 0;
        }
    
    int status;
    for (size_t i = 0; i < 2; i++)
        waitpid(pid[i], &status, 0);
    close(fd[0]);
    close(fd[1]);

    return 0;
}
