#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <wait.h>

#define READERS_CNT 4
#define WRITERS_CNT 3

#define ITER_CNT 32

#define ACTIVE_READER 0
#define ACTIVE_WRITER 1
#define WRITE_QUEUE 2
#define READ_QUEUE 3
#define BIN_WRITER 4

struct sembuf reader_lock[] = {{READ_QUEUE, 1, 0},
                               {ACTIVE_WRITER, 0, 0},
                               {WRITE_QUEUE, 0, 0},
                               {ACTIVE_READER, 1, 0},
                               {READ_QUEUE, -1, 0}};

struct sembuf reader_release[] = {{ACTIVE_READER, -1, 0}};

struct sembuf writer_lock[] = {{WRITE_QUEUE, 1, 0},
                               {ACTIVE_READER, 0, 0},
                               {ACTIVE_WRITER, 0, 0},
                               {ACTIVE_WRITER, 1, 0},
                               {BIN_WRITER, -1, 0},
                               {WRITE_QUEUE, -1, 0}};

struct sembuf writer_release[] = {{ACTIVE_WRITER, -1, 0},
                                  {BIN_WRITER, 1, 0}};

int start_read(int sem_id)
{
    return semop(sem_id, reader_lock, 5);
}

int stop_read(int sem_id)
{
    return semop(sem_id, reader_release, 1);
}

int start_write(int sem_id)
{
    return semop(sem_id, writer_lock, 6);
}

int stop_write(int sem_id)
{
    return semop(sem_id, writer_release, 2);
}

int reader_run(int *const addr, const int sem_id, const int rdid)
{
    srand(time(NULL) + rdid);

    for (size_t i = 0; i < ITER_CNT; i++)
    {
        sleep(rand() % 5 + 1);

        if (start_read(sem_id) == -1)
            return 1;

        printf("Reader %d read:  %d\n", rdid + 1, *addr);

        if (stop_read(sem_id) == -1)
            return 1;
    }

    return 0;
}

int writer_run(int *const addr, const int sem_id, const int wrid)
{
    for (size_t i = 0; i < ITER_CNT; i++)
    {
        sleep(rand() % 5 + 1);

        if (start_write(sem_id) == -1)
            return 1;

        printf("Writer %d write: %d\n", wrid + 1, ++(*addr));

        if (stop_write(sem_id) == -1)
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
    int fd = shmget(shmkey, sizeof(int), perms | IPC_CREAT);
    if (fd == -1)
    {
        perror("shmget");
        exit(1);
    }

    int *addr = shmat(fd, 0, 0);
    if (addr == (void *)-1)
    {
        perror("shmat");
        exit(1);
    }

    key_t semkey = ftok("/dev/null", 1);
    if (semkey == (key_t)-1)
    {
        perror("ftok (semkey)\n");
        exit(1);
    }

    int sem_id = semget(semkey, 5, perms | IPC_CREAT);
    if (sem_id == -1)
    {
        perror("semget");
        exit(1);
    }

    semctl(sem_id, ACTIVE_READER, SETVAL, 0);
    semctl(sem_id, ACTIVE_WRITER, SETVAL, 0);
    semctl(sem_id, WRITE_QUEUE, SETVAL, 0);
    semctl(sem_id, READ_QUEUE, SETVAL, 0);
    semctl(sem_id, BIN_WRITER, SETVAL, 1);

    for (size_t i = 0; i < READERS_CNT; i++)
    {
        int child_pid;
        if ((child_pid = fork()) == -1)
        {
            perror("can't fork (reader)");
            exit(1);
        }
        else if (child_pid == 0)
        {
            reader_run(addr, sem_id, i);
            return 0;
        }
    }

    for (size_t i = 0; i < WRITERS_CNT; i++)
    {
        int child_pid;
        if ((child_pid = fork()) == -1)
        {
            perror("can't fork (writer)");
            exit(1);
        }
        else if (child_pid == 0)
        {
            writer_run(addr, sem_id, i);
            return 0;
        }
    }

    for (size_t i = 0; i < READERS_CNT + WRITERS_CNT; i++)
    {
        int status;
        if (wait(&status) == -1)
            perror("Error with child process.\n");
        else if (!WIFEXITED(status))
            fprintf(stderr, "Children process %zu terminated abnormally.", i);
    }

    if (shmdt(addr) == -1 || shmctl(fd, IPC_RMID, NULL) == -1 || semctl(sem_id, IPC_RMID, 0) == -1)
    {
        perror("shmdt\n");
        exit(1);
    }

    return 0;
}
