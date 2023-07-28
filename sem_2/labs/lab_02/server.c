#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>

#define MAX_EVENTS_COUNT 100
#define SERVER_PORT 8080
#define MSG_LEN 64

static int sock_fd_global;

void server_shutdown(int signum)
{
    printf("\nShutdowning server...\n");
	close(sock_fd_global);
    exit(EXIT_SUCCESS);
}

int handle_event(int sock_fd)
{
	struct sockaddr_in client_addr; 
	socklen_t client_len;
	char input_msg[MSG_LEN];
	
	int bytes = read(sock_fd, input_msg, MSG_LEN);

	if (bytes == 0)
		return(EXIT_SUCCESS);

	if (bytes == -1)
	{
		perror("read");
		return EXIT_FAILURE;
	}

	printf("Server receive: %s \n", input_msg);

	char output_msg[MSG_LEN];
	sprintf(output_msg, "%s %d", input_msg, getpid());

	if (write(sock_fd, output_msg, MSG_LEN) == -1)
	{
		perror("write");
		return EXIT_FAILURE;
	}

	printf("Server send:    %s \n", output_msg);

	return EXIT_SUCCESS;
}

int main(void)
{
	setbuf(stdout, NULL);

	struct epoll_event ev, events[MAX_EVENTS_COUNT];
	int listen_sock, nfds, epoll_fd;

	struct sockaddr_in serv_addr =
	{
		.sin_family = AF_INET,
		.sin_addr.s_addr = INADDR_ANY,
		.sin_port = htons(SERVER_PORT)
	};
	
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}
	sock_fd_global = listen_sock;

	if (bind(listen_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
	{
		perror("bind");
		close(listen_sock);
		exit(EXIT_FAILURE);
	}

	if (listen(listen_sock, 1) == -1)
	{
		perror("listen");
		close(listen_sock);
		exit(EXIT_FAILURE);
	}

    signal(SIGINT, server_shutdown);
    signal(SIGTERM, server_shutdown);
    printf("Server is working.\n(Press Ctrl+C to stop)\n");

	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
	{
		perror("epoll_create1");
		close(listen_sock);
		exit(EXIT_FAILURE);
	}

	ev.events = EPOLLIN;
	ev.data.fd = listen_sock;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_sock, &ev) == -1)
	{
		perror("epoll_ctl: listen_sock");
		close(listen_sock);
		exit(EXIT_FAILURE);
	}

	for (;;)
	{
		nfds = epoll_wait(epoll_fd, events, MAX_EVENTS_COUNT, -1);
		if (nfds == -1)
		{
			perror("epoll_wait");
			close(listen_sock);
			exit(EXIT_FAILURE);
		}

		for (int n = 0; n < nfds; ++n)
		{
			if (events[n].data.fd == listen_sock)
			{
				struct sockaddr client_addr; 
				socklen_t client_len;
				
				int conn_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &client_len);
				
				if (conn_sock == -1)
				{
					perror("accept");
					close(listen_sock);
					exit(EXIT_FAILURE);
				}
				
				int status = fcntl(conn_sock, F_SETFL, fcntl(conn_sock, F_GETFL, 0) | O_NONBLOCK);
				if (status == -1)
				{
					perror("fcntl");
					close(listen_sock);
					exit(EXIT_FAILURE);
				}

				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = conn_sock;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
				{
					perror("epoll_ctl: conn_sock");
					close(listen_sock);
					exit(EXIT_FAILURE);
				}
			}
			else if (handle_event(events[n].data.fd) != EXIT_SUCCESS)
			{
				close(listen_sock);
				exit(EXIT_FAILURE);
			}
		}
	}

	close(listen_sock);

	return EXIT_SUCCESS;
}
