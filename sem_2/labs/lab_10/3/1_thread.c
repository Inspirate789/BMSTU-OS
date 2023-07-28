#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>

void *thread_func(void *fs)
{
    for (char c = 'b'; c <= 'z'; c += 2)
        fprintf((FILE *)fs, "subthread:   %c\n", c);

    return NULL;
}

int main(void)
{
    FILE *fs1 = fopen("1_thread.txt", "w");
    FILE *fs2 = fopen("1_thread.txt", "w");
    pthread_t thread;

    if (pthread_create(&thread, NULL, thread_func, fs2) != 0)
    {
        perror("pthread_create\n");
        exit(1);
    }

    for (char c = 'a'; c <= 'z'; c += 2)
        fprintf(fs1, "main thread: %c\n", c);

    if (pthread_join(thread, NULL) != 0)
    {
        perror("pthread_join\n");
        exit(1);
    }

    fclose(fs1);
    fclose(fs2);

    return 0;
}
