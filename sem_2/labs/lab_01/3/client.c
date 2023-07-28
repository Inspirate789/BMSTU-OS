#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define SOCKET_NAME "test_socket.s"
#define MSG_LEN 14

int main()
{
	int sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);

	if (sock_fd == -1)
	{
		perror("socket() failed");
		return EXIT_FAILURE;
	}

	struct sockaddr client_addr;
	client_addr.sa_family = AF_UNIX;
	char message[MSG_LEN];
	sprintf(message, "%d", getpid());
	strcpy(client_addr.sa_data, message);

	if (bind(sock_fd, &client_addr, sizeof(client_addr)) == -1)
	{
		perror("bind() failed");
		return EXIT_FAILURE;
	}

	struct sockaddr server_addr;
	server_addr.sa_family = AF_UNIX;
	strcpy(server_addr.sa_data, SOCKET_NAME);
	socklen_t server_len;

	if (sendto(sock_fd, message, strlen(message) + 1, 0, &server_addr, sizeof(server_addr)) < 0)
	{
		perror("sendto() failed");
		exit(1);
	}

	printf("Client send:    %d\n", getpid());

	int bytes = recvfrom(sock_fd, message, sizeof(message), 0, &server_addr, &server_len);

	if (bytes == -1)
	{
		perror("recvfrom() failed");
		exit(1);
	}

	printf("Client receive: %s \n", message);

	close(sock_fd);
	unlink(message);

	return 0;
}
