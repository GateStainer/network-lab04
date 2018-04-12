#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>


#include "user.h"


#define MAXLINE 4096
#define SERV_PORT 8888

int state = -1;


void printWelcome()
{
	system("clear");
	printf("Welcome to Battle Game online.\n");
	printf("(#) exit\t(h) help\n");
	printf("Please enter your user name:");
}

void printHelp()
{
	system("clear");
	printf("TODO: print help message\n");
	printf("(r) back\t(#) exit\n");
}

void* monitor_handler(void* socket_desc)
{
	char recvline[MAXLINE];
	int sock = *(int*)socket_desc;
	int read_size;
	while((read_size = recv(sock, recvline, MAXLINE, 0)) > 0)
	{

		TcpData* data = (TcpData*)recvline;
		switch(state){
			case 0:
				switch(data->category) {
					case 0x01:
						// 0x01 stands for login
						if(data->info == 0x01)
						{
							state = 3;
							printf("%s\n", "Successful login");
							printf("%s\n", "(#) exit");
						}
						else if(data->info == 0x02)
						{
							printf("%s\n", "Fail login. There exists user with same name\n");
						}
						else
						{
							printf("%s\n", "Unknown login response");
						}
						break;

					default:
						;
				}
				break;
			case 3:
				;
				break;
			default:
				;
		}
		memset(recvline, 0, MAXLINE);
	}
	return 0;

}


int main()
{
	int sockfd;
	struct sockaddr_in servaddr;
	char sendline[MAXLINE];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(8888);

	connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	state = 0;
	pthread_t thread_id;
	if(pthread_create(&thread_id, NULL, monitor_handler, (void*) &sockfd) > 0)
	{
		perror("Could not create thread");
		exit(-1);
	}

	printWelcome();
	while(fgets(sendline, MAXLINE, stdin) != NULL)
	{
		int i = strlen(sendline);
		if(sendline[i-1]=='\n')
			sendline[i-1] = '\0';
		if(strlen(sendline) == 0)
			continue;
		switch(state) {
			case 0:
				if(strcmp(sendline, "#")==0)
				{
					close(sockfd);
					exit(0);
				}
				else if(strcmp(sendline, "h")==0)
				{
					printHelp();
					state = 2;
				}
				else
				{
					//send login request
					TcpData data;
					data.category = 0x01;
					strcpy(data.user_name, sendline);
					send(sockfd, (char*)&data, sizeof(TcpData), 0);
				}
				break;
			case 2:
				if(strcmp(sendline, "#")==0)
				{
					close(sockfd);
					exit(0);
				}
				else if(strcmp(sendline, "r")==0)
				{
					printWelcome();
					state = 0;
				}
				break;
			case 3:
				if(strcmp(sendline, "#")==0)
				{
					close(sockfd);
					exit(0);
				}

				break;
			default:
				;
		}
		/*
		TcpData data;
		data.category = 0x01;
		strcpy(data.user_name, sendline);
		send(sockfd, (char*)&data, sizeof(TcpData), 0);
		*/
		memset(sendline, 0, MAXLINE);

	}
	exit(0);
}