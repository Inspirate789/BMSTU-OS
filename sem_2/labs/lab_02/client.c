#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define SERVER_PORT 8080
#define MSG_LEN 64

int main(void)
{
	setbuf(stdout, NULL);

	struct sockaddr_in serv_addr =
	{
		.sin_family = AF_INET,
		.sin_addr.s_addr = INADDR_ANY,
		.sin_port = htons(SERVER_PORT)
	};
	socklen_t serv_len;

	char buf[MSG_LEN];

	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("connect");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	char input_msg[MSG_LEN], output_msg[MSG_LEN];
	sprintf(output_msg, "%d", getpid());

	if (write(sock_fd, output_msg, strlen(output_msg) + 1) == -1)
	{
		perror("write");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	printf("Client send:    %d\n", getpid());

	if (read(sock_fd, input_msg, MSG_LEN) == -1)
	{
		perror("read");
		close(sock_fd);
		exit(EXIT_FAILURE);
	}

	printf("Client receive: %s \n", input_msg);

	close(sock_fd);

	return EXIT_SUCCESS;
}
