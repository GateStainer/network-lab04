#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "user.h"
extern map<string, user_info> user_data;

#define MAXLINE 4096
#define SERV_PORT 8888

void *connection_handler(void *);

int main()
{
	int socket_desc, client_sock, c;
	struct sockaddr_in server, client;

	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_desc == -1)
	{
		puts("Could not create socket");
		exit(-1);
	}
	puts("Socket created");

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(SERV_PORT);

	if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("Bind failed");
		exit(-1);
	}
	puts("Bind done");

	listen(socket_desc, 20);
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	pthread_t thread_id;

	while((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) != -1)
	{
		puts("Connection accepted");
		if(pthread_create(&thread_id, NULL, connection_handler, (void*) &client_sock) > 0)
		{
			perror("Could not create thread");
			exit(-1);
		}
		puts("Handler assigned");
	}
	perror("Accpet failed");
	close(socket_desc);
	return -1;
}

void* connection_handler(void* socket_desc)
{
	int sock = *(int*)socket_desc;
	int read_size;
	char *message;
	char client_message[MAXLINE];

	//message = "Greetings! I am your connection handler\n";
	//write(sock, message, strlen(message));

	while((read_size = recv(sock, client_message, MAXLINE, 0)) > 0)
	{
		client_message[read_size] = '\0';
		write(sock, client_message, strlen(client_message));
		memset(client_message, 0, MAXLINE);
	}
	if(read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("Receive failed");
	}
	return 0;

}
















