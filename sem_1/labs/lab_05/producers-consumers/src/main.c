#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <unistd.h>
#include <wait.h>

#define PROD_CNT 3
#define CONS_CNT 3

#define BIN_SEM 0
#define BUFF_FULL 1
#define BUFF_EMPTY 2

#define BUF_SIZE 64

typedef struct {
    char *caddr;
    char *paddr;
    char addr[BUF_SIZE];
} cbuffer_t;

struct sembuf cons_lock[] = {{BUFF_FULL, -1, 0},
                             {BIN_SEM, -1, 0}};

struct sembuf cons_release[] = {{BUFF_EMPTY, 1, 0},
                                {BIN_SEM, 1, 0}};

struct sembuf prod_lock[] = {{BUFF_EMPTY, -1, 0},
                             {BIN_SEM, -1, 0}};

struct sembuf prod_release[] = {{BUFF_FULL, 1, 0},
                                {BIN_SEM, 1, 0}};

int consumer_run(cbuffer_t *const buf, const int sem_id, const int cons_id)
{
    srand(time(NULL) + cons_id);

    for (int i = 0; i < 8; i++)
    {
        sleep(rand() % 3 + 1);

        if (semop(sem_id, cons_lock, 2) == -1)
            return 1;

        char symb = *(buf->caddr);
        (buf->caddr)++;

        printf("Consumer %d read:  %c\n", cons_id + 1, symb);

        if (semop(sem_id, cons_release, 2) == -1)
            return 1;
    }

    return 0;
}

int producer_run(cbuffer_t *const buf, const int sem_id, const int prod_id)
{
    srand(time(NULL) + prod_id);

    for (size_t i = 0; i < 8; i++)
    {
        sleep(rand() % 9 + 1);

        if (semop(sem_id, prod_lock, 2) == -1)
            return 1;

        *(buf->paddr) = *(buf->paddr - 1) + 1;
        printf("Producer %d write: %c\n", prod_id + 1, *(buf->paddr++));

        if (semop(sem_id, prod_release, 2))
            return 1;
    }

    return 0;
}

int main(void)
{
    setbuf(stdout, NULL);

    key_t shmkey = ftok("/dev/null", 1);
    if (shmkey == (key_t)-1)
    {
        perror("ftok (shmkey)\n");
        exit(1);
    }

    int perms = S_IRWXU | S_IRWXG | S_IRWXO;
    int fd = shmget(shmkey, sizeof(cbuffer_t), perms | IPC_CREAT);
    if (fd == -1)
    {
        perror("shmget\n");
        exit(1);
    }

    cbuffer_t *buffer = shmat(fd, 0, 0);
    if (buffer == (void *)-1)
    {
        perror("shmat\n");
        exit(1);
    }

    buffer->caddr = buffer->addr + 1;
    buffer->paddr = buffer->addr + 1;
    *(buffer->addr) = 'a' - 1;

    key_t semkey = ftok("/dev/null", 1);
    if (semkey == (key_t)-1)
    {
        perror("ftok (semkey)\n");
        exit(1);
    }

    int sem_id = semget(semkey, 3, perms | IPC_CREAT);
    if (-1 == sem_id)
    {
        perror("semget\n");
        exit(1);
    }

    semctl(sem_id, BIN_SEM, SETVAL, 1);
    semctl(sem_id, BUFF_FULL, SETVAL, 0);
    semctl(sem_id, BUFF_EMPTY, SETVAL, BUF_SIZE);

    for (size_t i = 0; i < PROD_CNT; i++)
    {
        int child_pid;
        if ((child_pid = fork()) == -1)
        {
            perror("can't fork (producer)");
            exit(1);
        }
        else if (child_pid == 0)
        {
            producer_run(buffer, sem_id, i);
            return 0;
        }
    }

    for (size_t i = 0; i < CONS_CNT; i++)
    {
        int child_pid;
        if ((child_pid = fork()) == -1)
        {
            perror("can't fork (consumer)");
            exit(1);
        }
        else if (child_pid == 0)
        {
            consumer_run(buffer, sem_id, i);
            return 0;
        }
    }

    for (size_t i = 0; i < PROD_CNT + CONS_CNT; i++)
    {
        int status;
        if (wait(&status) == -1)
            perror("Error with child process.\n");
        else if (!WIFEXITED(status))
            fprintf(stderr, "Children process %zu terminated abnormally.", i);
    }

    if (shmdt(buffer) == -1 || shmctl(fd, IPC_RMID, NULL) == -1 || semctl(sem_id, IPC_RMID, 0) == -1)
    {
        perror("shmdt\n");
        exit(1);
    }

    return 0;
}
