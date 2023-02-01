#include <unistd.h>

#include "bakery.h"

#define CLIENTS_COUNT 7

void bakery_prog_1(char *host, int index)
{
	CLIENT *clnt;
	struct REQUEST request;
	request.index = index;
	request.pid = getpid();
	int *result_1, *result_2;

#ifndef DEBUG
	clnt = clnt_create(host, BAKERY_PROG, BAKERY_VER, "udp");
	if (clnt == NULL)
	{
		clnt_pcreateerror(host);
		exit(1);
	}
#endif /* DEBUG */

	usleep(350 * request.index);

	result_1 = get_number_1(&request, clnt);
	if (result_1 == (int *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}

	request.number = *result_1;
	printf("Client with index  %d (pid: %d) got number: %d\n", request.index, request.pid, request.number);

	result_2 = bakery_service_1(&request, clnt);
	if (result_2 == (int *)NULL)
	{
		clnt_perror(clnt, "call failed");
	}

	printf("Client with number %d (pid: %d) got result: %d\n", request.number, request.pid, *result_2);

#ifndef DEBUG
	clnt_destroy(clnt);
#endif /* DEBUG */
}

int main(int argc, char *argv[])
{
	char *host;

	if (argc < 2)
	{
		printf("usage: %s server_host\n", argv[0]);
		exit(1);
	}
	host = argv[1];

	pid_t client_pid[CLIENTS_COUNT];

	for (size_t i = 0; i < CLIENTS_COUNT; i++)
	{
		if ((client_pid[i] = fork()) == -1)
		{
			perror("Can't fork\n");
			exit(1);
		}
		else if (client_pid[i] == 0)
		{
			bakery_prog_1(host, i);
			return 0;
		}
	}

	for (size_t i = 0; i < CLIENTS_COUNT; i++)
	{
		int status;
		waitpid(client_pid[i], &status, 0);
	}

	printf("Parent client exited\n");

	exit(0);
}
