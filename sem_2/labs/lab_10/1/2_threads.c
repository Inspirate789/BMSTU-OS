#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>

void *thread_func1(void *fs)
{
    char c;
    while (fscanf((FILE *)fs, "%c", &c) == 1)
        fprintf(stdout, "subthread 1:   %c\n", c);

    return NULL;
}

void *thread_func2(void *fs)
{
    char c;
    while (fscanf((FILE *)fs, "%c", &c) == 1)
        fprintf(stdout, "subthread 2:   %c\n", c);

    return NULL;
}

int main(void)
{
    int fd = open("alphabet.txt", O_RDONLY);
    FILE *fs[2] = {fdopen(fd, "r"), fdopen(fd, "r")};
    char buff[2][20];
    setvbuf(fs[0], buff[0], _IOFBF, 20);
    setvbuf(fs[1], buff[1], _IOFBF, 20);
    char c;
    pthread_t threads[2];
    void *(*thread_funcs[2])(void *) = {thread_func1, thread_func2};

    for (size_t i = 0; i < 2; i++)
        if (pthread_create(&threads[i], NULL, thread_funcs[i], fs[i]) != 0)
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
