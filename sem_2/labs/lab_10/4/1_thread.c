#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mx;

void *thread_func(void *fd)
{
    for (char c = 'n'; c <= 'z'; c++)
    {
        pthread_mutex_lock(&mx);
        write((int)fd, &c, 1);
        pthread_mutex_unlock(&mx);
    }
    
    return NULL;
}

int main(void)
{
    int fd1 = open("1_thread.txt", O_CREAT | O_WRONLY);
    int fd2 = open("1_thread.txt", O_CREAT | O_WRONLY | O_APPEND);
    pthread_t thread;

    if (pthread_mutex_init(&mx, NULL) != 0)
    {
        perror("pthread_mutex_init\n");
        exit(1);
    }

    if (pthread_create(&thread, NULL, thread_func, fd2) != 0)
    {
        perror("pthread_create\n");
        exit(1);
    }

    for (char c = 'a'; c <= 'm'; c++)
    {
        pthread_mutex_lock(&mx);
        write(fd1, &c, 1);
        pthread_mutex_unlock(&mx);
    }

    if (pthread_join(thread, NULL) != 0)
    {
        perror("pthread_join\n");
        exit(1);
    }

    if (pthread_mutex_destroy(&mx) != 0)
    {
        perror("pthread_mutex_destroy\n");
        exit(1);
    }

    close(fd1);
    close(fd2);
    return 0;
}
