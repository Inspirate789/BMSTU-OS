#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>

void *thread_func1(void *fs)
{
    for (char c = 'a'; c <= 'z'; c += 2)
        fprintf((FILE *)fs, "subthread 1: %c\n", c);

    return NULL;
}

void *thread_func2(void *fs)
{
    for (char c = 'b'; c <= 'z'; c += 2)
        fprintf((FILE *)fs, "subthread 2: %c\n", c);

    return NULL;
}

int main(void)
{
    FILE *fs[2] = {fopen("2_threads.txt", "w"), fopen("2_threads.txt", "w")};
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
    
    fclose(fs[0]);
    fclose(fs[1]);

    return 0;
}
