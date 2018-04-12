#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <iterator>
#include <set>

#include "user.h"

extern unordered_map<string, struct user_info> user_table;


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


	while((read_size = recv(sock, client_message, MAXLINE, 0)) > 0)
	{
		TcpData* data = (TcpData*)client_message;
		switch(data->category) {
			case 0x01:
				/* 0x01 stands for login
				first check whether there is an online user with same name
				if not, add the user into user_table or change user state to 1, else return bad request*/
				if(user_table.find(data->user_name)==user_table.end() || user_table[data->user_name].state!=1)
				{
					UserInfo userinfo;
					userinfo.state = 1;
					userinfo.current_life = 10;
					userinfo.sock = sock;
					user_table.insert({data->user_name, userinfo});
					data->info = 0x01; //success
					printf("%s\n", "Success login");
					printf("Total user number: %d\n", user_table.size());
					write(sock, data, sizeof(TcpData));
					
				}
				else
				{
					data->info = 0x02; //fail to login
					printf("%s\n", "Fail login");
					write(sock, data, sizeof(TcpData));
				}
				break;

			default:
				;
		}
		memset(client_message, 0, MAXLINE);
	}
	if(read_size == 0)
	{
		unordered_map<string, UserInfo>::iterator it = user_table.begin();
		while(it != user_table.end())
		{
			if(it->second.sock == sock)
			{
				it->second.state = 0; // offline
				printf("User %s offline\n", &it->first[0]);
				break;
			}
			it++;
		}
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("Receive failed");
	}
	close(sock);
	return 0;

}


void broadcast()
{
	
}















