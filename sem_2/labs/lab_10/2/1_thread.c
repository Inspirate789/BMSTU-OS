#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

void *thread_func(void *fd)
{
    char c;
    while (read((int)fd, &c, 1) == 1)
        write(1, &c, 1);

    return NULL;
}

int main(void)
{
    int fd1 = open("alphabet.txt", O_RDONLY);
    int fd2 = open("alphabet.txt", O_RDONLY);
    char c;
    pthread_t thread;

    if (pthread_create(&thread, NULL, thread_func, fd2) != 0)
    {
        perror("pthread_create\n");
        exit(1);
    }
    
    while (read(fd1, &c, 1) == 1)
        write(1, &c, 1);
    
    if (pthread_join(thread, NULL) != 0)
    {
        perror("pthread_join\n");
        exit(1);
    }

    return 0;
}
