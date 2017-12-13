#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>

void data(char *data, char *message, int *m, int *n)
{
	char
		str[256] = "",
		*token,
		token1[256] = "",
		token2[256] = "";

	int k = 0;

	strcpy(str, data + 5);

	token = strtok(str, " ,");

	while (token != NULL)
	{
		k++;

		if (k == 1)
			strcpy(token1, token);

		if (k == 2)
			strcpy(token2, token);

		token = strtok(NULL, " ,");
	}

	if (*n != -1 && *m != -1)
		sprintf(message, "510 Variables entered");
	else if (k > 2)
		sprintf(message, "540  You must enter 2 variables");
	else if (k < 2)
		sprintf(message, "530 You must enter 2 variables");
	else if (k == 2)
	{
		int i = 0, I = 0, check = 0;

		char
			variable1[256] = "",
			value1[256] = "",
			variable2[256] = "",
			value2[256] = "";

		while (token1[i])
		{
			if (token1[i] == '=')
			{
				I = i;
				check++;
			}
			i++;
		}

		if (check == 1)
		{
			strncpy(variable1, token1, I);
			strcpy(value1, token1 + I + 1);
		}

		i = 0; I = 0; check = 0;

		while (token2[i])
		{
			if (token2[i] == '=')
			{
				I = i;
				check++;
			}
			i++;
		}

		if (check == 1)
		{
			strncpy(variable2, token2, I);
			strcpy(value2, token2 + I + 1);
		}

		int negative1 = 0, negative2 = 0; i = 1;

		int val1 = atoi(value1), val2 = atoi(value2);

		if (strncmp(variable1, "n", 1) == 0 && strncmp(variable2, "m", 1) == 0)
		{
			if (val1 >= 0 && val2 >= 0 && val1 < 100 && val2 < 100)
			{
				*n = val1;
				*m = val2;

				sprintf(message, "250 N = %d, M = %d", *n, *m);
			}
			else
				sprintf(message, "520 Values must be from 1 to 100");
		}
		else if (strcmp(variable1, "m") == 0 && strcmp(variable2, "n") == 0)
		{
			if (val1 >= 0 && val2 >= 0 && val1 < 100 && val2 < 100)
			{
				*m = val1;
				*n = val2;

				sprintf(message, "250 N = %d, M = %d", *n, *m);
			}
			else
				sprintf(message, "520 Values must be from 1 to 100");
		}
		else
			sprintf(message, "550 Error");
	}
}

void res( int m, int n, char *message)
{
	if(m == -1 || n == -1)
		strcpy(message,  "540 Please enter M and N");
	else if(m >= 0 && n >= 0 && m <= 100 && n <= 100)
	{
		int s = 0;
		for (int i = m; i <= n; i++)
			if(i%2 != 0 && i%7 == 0) s += i;
		
		char str[256] = "";
		sprintf(str, "220 Result is %d\n250", s);
		strcpy(message,  str);
	}		
	else
		strcpy(message,  "550 Error");
}

void rset( int *m, int *n, char *message)
{
	*m = -1;
	*n = -1;
	strcpy(message, "250 Reset");
}

int main()
{
	int
		server = socket(AF_INET, SOCK_DGRAM, 0),	
		port = 1998;
	
	if (server < 0)
	{
		perror("socket creating error\n");
		exit(1);
	}

	char ans[10] = "";

	while (strcmp(ans,"y") != 0 && strcmp(ans,"n") != 0)
	{
		printf("Do you want to enter the port? (y/n): ");
		scanf("%s", ans);
	}

	if (strcmp(ans,"y") == 0)
	{
		printf("Enter the port: ");
		scanf("%d", &port);
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(server, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("connecting error\n");
		exit(2);
	}

	int 
		bytes = 0,
		N = -1,
		M = -1;

	char 
		clientMessage[256] = "",
		serverMessage[256] = "";

	socklen_t fromlen;
	struct sockaddr_in addrc;

	while (1)
	{
		strcpy(serverMessage, "");

		while (1)
		{
			bytes = recvfrom(server, clientMessage, sizeof(clientMessage), 0, (struct sockaddr *)&addrc, &fromlen);

			strcpy(clientMessage, strtok(clientMessage, "\n"));

			if (bytes <= 0)
				break;

			else if (strcmp(clientMessage, "first") == 0)
				strcpy(serverMessage, "250 V.24.02.1998 My_Little_Server");

			else if (strcmp(clientMessage, "var")==0)
				strcpy(serverMessage, "220 Variables\nM\n0 ... 100\nN\n0 ... 100\n250 :-)");

			else if (strncmp(clientMessage, "data ", 5) == 0 || (strncmp(clientMessage, "data", 4) == 0 && clientMessage[4] == '\0'))
				data(clientMessage, serverMessage, &M, &N);

			else if (strcmp(clientMessage, "res")==0)
				res(M, N, serverMessage);

			else if (strcmp(clientMessage, "rset")==0)
				rset(&M, &N, serverMessage);

			else if (strcmp(clientMessage,"quit")==0)
				strcpy(serverMessage, "250 Work is completed");

			else 
				strcpy(serverMessage, "Unknown");			
						
			printf("%s\n", serverMessage);
			sendto(server, serverMessage, sizeof(serverMessage), 0, (struct sockaddr *)&addrc, fromlen);
		}		
	}

	return 0;
}


