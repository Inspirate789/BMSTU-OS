#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>

sem_t sem;

int main(void)
{
    int fd1 = open("1_subproc.txt", O_CREAT | O_WRONLY);
    int fd2 = open("1_subproc.txt", O_CREAT | O_WRONLY);
    write(1, &fd1, sizeof(fd1));
    write(1, &fd2, sizeof(fd2));
    pid_t pid;

    if (sem_init(&sem, 0, 1) == -1)
    {
        perror("sem_init\n");
        exit(1);
    }

    if ((pid = fork()) == -1)
    {
        perror("fork\n");
        exit(1);
    }
    else if (pid == 0)
    {
        sem_wait(&sem);
        for (char c = 'n'; c <= 'z'; c++)
            write(fd2, &c, 1);
        sem_post(&sem);
        return 0;
    }
    
    usleep(10);
    sem_wait(&sem);
    for (char c = 'a'; c <= 'm'; c++)
        write(fd1, &c, 1);
    sem_post(&sem);
    
    if (sem_destroy(&sem) == -1)
    {
        perror("sem_destroy\n");
        exit(1);
    }
    
    int status;
    waitpid(pid, &status, 0);
    close(fd1);
    close(fd2);

    return 0;
}
