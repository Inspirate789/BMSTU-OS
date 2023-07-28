#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mx;

void *thread_func1(void *fd)
{
    for (char c = 'a'; c <= 'm'; c++)
    {
        pthread_mutex_lock(&mx);
        write((int)fd, &c, 1);
        pthread_mutex_unlock(&mx);
    }

    return NULL;
}

void *thread_func2(void *fd)
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
    int fd[2] = {open("2_threads.txt", O_CREAT | O_WRONLY), 
                 open("2_threads.txt", O_CREAT | O_WRONLY | O_APPEND)};
    pthread_t threads[2];
    void *(*thread_funcs[2])(void *) = {thread_func1, thread_func2};

    if (pthread_mutex_init(&mx, NULL) != 0)
    {
        perror("pthread_mutex_init\n");
        exit(1);
    }

    for (size_t i = 0; i < 2; i++)
        if (pthread_create(&threads[i], NULL, thread_funcs[i], fd[i]) != 0)
        {
            perror("pthread_create\n");
            exit(1);
        }

    for (size_t i = 0; i < 2; i++)
        if (pthread_join(threads[i], NULL) != 0)
        {
            perror("pthread_join\n");
            exit(1);
        }

    if (pthread_mutex_destroy(&mx) != 0)
    {
        perror("pthread_mutex_destroy\n");
        exit(1);
    }
    
    close(fd[0]);
    close(fd[1]);

    return 0;
}
