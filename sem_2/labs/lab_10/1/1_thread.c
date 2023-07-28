#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>

void *thread_func(void *fs)
{
    char c;
    while (fscanf((FILE *)fs, "%c", &c) == 1)
        fprintf(stdout, "subthread:   %c\n", c);

    return NULL;
}

int main(void)
{
    int fd = open("alphabet.txt", O_RDONLY);
    FILE *fs1 = fdopen(fd, "r"), *fs2 = fdopen(fd, "r");
    char buff1[20], buff2[20];
    setvbuf(fs1, buff1, _IOFBF, 20);
    setvbuf(fs2, buff2, _IOFBF, 20);
    char c;
    pthread_t thread;

    if (pthread_create(&thread, NULL, thread_func, fs2) != 0)
    {
        perror("pthread_create\n");
        exit(1);
    }

    while (fscanf(fs1, "%c", &c) == 1)
        fprintf(stdout, "main thread: %c\n", c);

    if (pthread_join(thread, NULL) != 0)
    {
        perror("pthread_join\n");
        exit(1);
    }

    return 0;
}
