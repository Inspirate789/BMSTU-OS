#include <stdio.h>
#include <stdlib.h> // rand()
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>

struct sembuf sbuf1[1] = {{0, -1, 1}};
struct sembuf sbuf2[1] = {{0, 1, 1}};

int main(void)
{

	int perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	// IPC_PRIVATE - ключ, который показывает, что
	// Набор семафоров могут использовать только процессы,
	// Порожденные процессом, создавшим семафор.
	// Создаем новый набор 3 семафоров.
	int fd = semget(IPC_PRIVATE, 2, IPC_CREAT | perms);

	if (fd == -1)
	{
		perror("semget");
		exit(1);
	}

	pid_t childpid;
	if ((childpid = fork()) == -1)
	{
		perror("cannot fork");
		exit(1);
	}
	else if (childpid == 0)
	{
		printf("child begin\n");

		int rc = semop(fd, sbuf1, 1);

		printf("return code = %d\n", rc);

		if (rc == -1)
		{
			perror("semop child");
			exit(1);
		}

		printf("child ok\n");
		exit(0);
	}

	sleep(1);

	if (semop(fd, sbuf2, 1) == -1)
	{
		perror("semop parent");
		return 1;
	}

	printf("parent ok\n");

	return 0;
}