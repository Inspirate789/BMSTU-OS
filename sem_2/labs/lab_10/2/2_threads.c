#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

void *thread_func1(void *fd)
{
    char c;
    while (read((int)fd, &c, 1) == 1)
        write(1, &c, 1);

    return NULL;
}

void *thread_func2(void *fd)
{
    char c;
    while (read((int)fd, &c, 1) == 1)
        write(1, &c, 1);

    return NULL;
}

int main(void)
{
    int fd[2] = {open("alphabet.txt", O_RDONLY), open("alphabet.txt", O_RDONLY)};
    char c;
    pthread_t threads[2];
    void *(*thread_funcs[2])(void *) = {thread_func1, thread_func2};

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

    return 0;
}
