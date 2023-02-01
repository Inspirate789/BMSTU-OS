#include <stdbool.h>

#include "bakery.h"

static int counter = 0;

static bool choosing[MAX_CLIENTS_COUNT] = {false};
static int numbers[MAX_CLIENTS_COUNT] = {0};
static int pids[MAX_CLIENTS_COUNT] = {0};

int max_number()
{
	int res = numbers[0];

	for (int i = 1; i < MAX_CLIENTS_COUNT; i++)
		if (numbers[i] > res)
			res = numbers[i];

	return res;
}

int *
get_number_1_svc(struct REQUEST *argp, struct svc_req *rqstp)
{
	static int  result;
	printf("Client with index  %d (pid: %d) logged to server\n", argp->index, argp->pid);

	pids[argp->index] = argp->pid;

	choosing[argp->index] = true;
	result = numbers[argp->index] = max_number() + 1;
	choosing[argp->index] = false;

	return &result;
}

int *
bakery_service_1_svc(struct REQUEST *argp, struct svc_req *rqstp)
{
	static int  result;

	for (int i = 0; i < MAX_CLIENTS_COUNT; i++)
	{
		while (choosing[i]) {}
		while (numbers[i] != 0 && (numbers[i] < numbers[argp->index] || 
								   numbers[i] == numbers[argp->index] && pids[i] < pids[argp->index])) {}
	}

	// Начало критической секции
	result = counter++;
	printf("Client with number %d (pid: %d) got result: %d\n", numbers[argp->index], pids[argp->index], result);
	numbers[argp->index] = 0;
	// Конец критической секции

	return &result;
}
