#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

char quitMessage[256] = "250 Work is completed";

int main()
{
	int
		port = 1998,
		client = socket(AF_INET, SOCK_DGRAM, 0);

	if (client < 0)
	{
		perror("socket creating error\n");
		exit(1);
	}

	char ans[10] = "";

	while (strcmp(ans, "y") != 0 && strcmp(ans, "n") != 0 )
	{
		printf("\e[1m\x1b[35mDo you want to enter the port? (y/n)\n>: \x1b[0m\e[0m");
		scanf("%s", ans);
	}

	if (strcmp(ans, "y") == 0)
	{
		printf("\e[1m\x1b[35mEnter the port\n>: \x1b[0m\e[0m");
		scanf("%d", &port);
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	char 
		clientMessage[256] = "",
		serverMessage[256] = "",
		helloMessage[256] = "";

	sendto(client, "first", sizeof("first"), 0, (struct sockaddr *)&addr, sizeof(addr));
	recvfrom(client, helloMessage, sizeof(helloMessage), 0, (struct sockaddr *)&addr, (socklen_t *)sizeof(addr));
		
	printf("\e[1m\x1b[35m%s\n>: \x1b[0m\e[0m", helloMessage);		
	fgets(clientMessage, 256, stdin);

	while (strcmp(serverMessage, quitMessage) != 0)
	{		
		fgets(clientMessage, 256, stdin);	

		sendto(client, clientMessage, sizeof(clientMessage), 0, (struct sockaddr *)&addr, sizeof(addr));
		recvfrom(client, serverMessage, sizeof(serverMessage), 0, (struct sockaddr *)&addr, (socklen_t *)sizeof(addr));			 	
		
		printf("\e[1m\x1b[35m%s\n>: \x1b[0m\e[0m", serverMessage);		
	}

	close(client);
	return 0;
}


