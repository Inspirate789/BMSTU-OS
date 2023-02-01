#include <stdio.h>
// #include <pthread.h>
// #include <semaphore.h>
// #include <unistd.h>

// int main(void)
// {
// 	int N = 24; /*размер буфера*/
// 	sem_t buffer_full, buffer_empty, bin_sem;

// 	sem_init(&buffer_empty, 1, N);

// 	return OK;
// }

#include <stdlib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>

// struct sembuf
// {
// 	ushort sem_num; /* индекс семафора в массиве */
// 	short sem_op;	/* операция над семафором */
// 	short sem_flg;	/* флаги */
// };

struct sembuf sbuf[2] = {{0, 1, SEM_UNDO | IPC_NOWAIT}, 
                         {0, -1, SEM_UNDO | IPC_NOWAIT}};

int main(void)
{
	int perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	int fd = semget(100, 2, IPC_CREAT | perms);

	if (fd == -1)
	{
		perror("semget");
		exit(1);
	}
	if (semop(fd, sbuf, 2) == -1)
	{
		perror("semop");
		exit(1);
	}
    
	printf("ok\n");
	return 0;
}
