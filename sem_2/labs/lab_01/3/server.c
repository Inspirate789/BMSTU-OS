#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define SERV_SOCKET_NAME "test_socket.s"
#define MSG_LEN 14

static int sock_fd_global;

void clean()
{
	close(sock_fd_global);
	unlink(SERV_SOCKET_NAME);
}

void sighandler(int signum)
{
    printf("\nShutdowning server...\n");
	close(sock_fd_global);
	unlink(SERV_SOCKET_NAME);
    exit(0);
}

int main()
{
	int sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	sock_fd_global = sock_fd;

	if (sock_fd == -1)
	{
		perror("Create socket error");
		exit(1);
	}

	struct sockaddr server_addr;
	server_addr.sa_family = AF_UNIX;
	strcpy(server_addr.sa_data, SERV_SOCKET_NAME);

	if (bind(sock_fd, &server_addr, sizeof(server_addr)) == -1)
	{
		perror("Bind error");
	    close(sock_fd);
		exit(1);
	}

    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    printf("Server is working.\n(Press Ctrl+C to stop)\n");

	struct sockaddr client_addr; 
	socklen_t client_len;

	char input_msg[MSG_LEN];
	while(1)
	{
		int bytes = recvfrom(sock_fd, input_msg, sizeof(input_msg), 0, &client_addr, &client_len);

		if (bytes == -1)
		{
			perror("recvfrom error");
			clean();
			unlink(input_msg);
			exit(1);
		}

		printf("Server receive: %s \n", input_msg);

        char output_msg[MSG_LEN];
        sprintf(output_msg, "%s %d", input_msg, getpid());

        if (sendto(sock_fd, output_msg, strlen(output_msg) + 1, 0, &client_addr, sizeof(client_addr)) == -1)
        {
            perror("sendto failed");
			clean();
			unlink(input_msg);
            exit(1);
        }

		printf("Server send:    %s \n", output_msg);

		unlink(input_msg);
	}

	return 0;
}
