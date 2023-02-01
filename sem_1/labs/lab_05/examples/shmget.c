#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/shm.h>

int main(void)
{
    int shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | S_IRWXU | S_IRWXG | S_IRWXO);
	if (shmid == -1)
	{
		perror("Unable to create shared area");
		exit(1);
	}

	int *addr = shmat(shmid, NULL, 0);
	*addr = 5;

	printf("parent begin: %d (address: %p)\n", *addr, addr);

	pid_t childpid;
	if ((childpid = fork()) == -1)
	{
		perror("cannot fork");
		exit(-1);
	}
	else if (childpid == 0)
	{
		printf("child begin\n");
		*addr = 33;
		printf("child end: %d (address: %p)\n", *addr, addr);
		exit(0);
	}

	sleep(2);

	printf("parent end: %d (address: %p)\n", *addr, addr);

	return 0;
}