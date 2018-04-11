#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "user.h"


#define MAXLINE 4096
#define SERV_PORT 8888

int main()
{
	int sockfd;
	struct sockaddr_in servaddr;
	char sendline[MAXLINE], recvline[MAXLINE];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(8888);

	connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	while(fgets(sendline, MAXLINE, stdin) != NULL)
	{
		send(sockfd, sendline, strlen(sendline), 0);
		if(recv(sockfd, recvline, MAXLINE, 0) == 0)
		{
			perror("The server terminated");
			exit(4);
		}
		printf("%s", "String received from server: ");
		fputs(recvline, stdout);

		memset(sendline, 0, MAXLINE);
		memset(recvline, 0, MAXLINE);

	}
	exit(0);
}