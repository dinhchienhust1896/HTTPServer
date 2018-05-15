// HTTP Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "HTTP Server.h"

int main()
{
	int res;

	// Initiate WinSock
	WSADATA wsaData;
	res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (res == SOCKET_ERROR)
	{
		printf("Version is not supported!\n");
		Log("Version is not supported!", "ERROR");
		getchar();
		return FALSE;
	}
	// Log
	Log("Initiate Winsock", "INFO");

	// Construct socket
	SOCKET listener;
	listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Bind address to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	if (bind(listener, (sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		printf("bind() failed!. Error code : %d\n", WSAGetLastError());
		Log("bind() failed!", "ERROR");
		getchar();
		return FALSE;
	}
	// Log
	char info[50] = "";
	sprintf(info, "Bind [%s:%d]", SERVER_ADDR, SERVER_PORT);
	Log(info, "INFO");
	//Listen request from client
	if (listen(listener, 10)) {
		printf("listen() failed!. Error code : %d\n", WSAGetLastError());
		Log("listen() failed!", "ERROR");
		getchar();
		return FALSE;
	}
	// Log
	memset(info, 0, sizeof(info));
	sprintf(info, "Listen [%s:%d]", SERVER_ADDR, SERVER_PORT);
	Log(info, "INFO");

	printf("[%s:%d] open!\n", SERVER_ADDR, SERVER_PORT);

	//Communicate with client
	while (1)
	{
		struct sockaddr_in clientAddr;
		int c = sizeof(struct sockaddr_in);
		SOCKET client;
		client = accept(listener, (struct sockaddr *)&clientAddr, &c);

		// Get client IP, Port
		char *client_ip = inet_ntoa(clientAddr.sin_addr);
		int client_port = ntohs(clientAddr.sin_port);

		// Log
		char msg[50];
		sprintf(msg, "Client [%s:%d] accepted!. SOCKET : %d", client_ip, client_port, client);
		Log(msg, "INFO");

		printf("Client [%s:%d] accepted!. SOCKET : %d\n", client_ip, client_port, client);
		// Create new thread when a client connected!
		CreateThread(0, 0, ClientThread, &client, 0, 0);
	}
	closesocket(listener);
	WSACleanup();
	return TRUE;
}

DWORD WINAPI ClientThread(LPVOID lpParams)
{
	SOCKET client = *(SOCKET *)lpParams;
	char buffer[1024] = "";
	int res;
	// Process message
	//----------------------------------------------
	while (1)
	{
		res = recv(client, buffer, sizeof(buffer), 0);
		if (res == 0 || res == SOCKET_ERROR)
		{
			closesocket(client);
			return FALSE;
		}
		buffer[res] = 0;
		// Check HTTP Request
		if (strstr(buffer, " HTTP/") == NULL)
		{
			printf("Client %d : Not HTTP Request!\n", client);
			return FALSE;
		}

		// Detect Method
		if (strncmp(buffer, "GET", 3) == 0) {
			// GET Method
			GET_METHOD(client, buffer);
		}
		else if (strncmp(buffer, "POST", 4) == 0)
		{
			// POST Method
			POST_METHOD(client, buffer);
		}
		else if (strncmp(buffer, "PUT", 3) == 0)
		{
			// PUT Method
			PUT_METHOD(client, buffer);
		}
		else if (strncmp(buffer, "HEAD", 4) == 0)
		{
			// HEAD Method
			HEAD_METHOD(client, buffer);
		}
		else if (strncmp(buffer, "DELETE", 6) == 0)
		{
			DELETE_METHOD(client, buffer);
		}
		else
		{
			// Unknown Method
			UNKNOWN_METHOD(client, buffer);
		}
	}
	closesocket(client);
	return TRUE;
}

void GET_METHOD(SOCKET client, char * request)
{

	// Get URI
	char *start = strchr(request, '/');
	char *ptr = strstr(request, "HTTP/");
	char uri[512]="";
	memcpy(uri, start, ptr - start - 1);
	// Print
	printf("SOCKET %d : GET method. URI : %s\n", client, uri);
	// Log
	char msg[50];
	sprintf(msg, "SOCKET %d : GET method!. URI : %s", client, uri);
	Log(msg, "INFO");
	// Send to Client
	if (strcmp(uri, "/") == 0)
	{
		// Send Index.html
		send(client, RESPONSE_STATUS_LINE, strlen(RESPONSE_STATUS_LINE), 0);
		send(client, RESPONSE_CONTENT_TYPE, strlen(RESPONSE_CONTENT_TYPE), 0);
		FILE *index = fopen("Index.html", "r");
		if (index == NULL)
		{
			printf("Can't open file : Index.html\n");
			SEND_404(client);
			return;
		}
		fseek(index, 0, SEEK_END);
		int size = ftell(index);
		char msg[50] = "";
		sprintf(msg, "Content-Length: %d\n\n", size);
		send(client, msg, strlen(msg), 0);
		//// Send content
		fseek(index, 0, SEEK_SET);
		char line[512];
		while (fgets(line, sizeof(line), index) != NULL) {
			// Replace \n => \r\n
			int i = 0;
			while (line[i] != '\0')
			{
				if (line[i] == '\n')
				{
					line[i] = '\r';
					line[i + 1] = '\n';
					line[i + 2] = '\0';
					i++;
				}
				i++;
			}
			send(client, line, strlen(line), 0);
		}
		fclose(index);
		return;
	}
	else
	{
		char path[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, path);
		// Chang \ to /
		int i = 0;
		while (uri[i] != '\0')
		{
			if (uri[i] == '/')
			{
				uri[i] = '\\';
			}
			i++;
		}
		strcat(path, uri);
		// Try open file
		FILE *file = fopen(path, "r");
		if (file == NULL)
		{
			printf("Can't open file : %s\n", path);
			SEND_404(client);
			return;
		}

		send(client, RESPONSE_STATUS_LINE, strlen(RESPONSE_STATUS_LINE), 0);
		send(client, RESPONSE_CONTENT_TYPE, strlen(RESPONSE_CONTENT_TYPE), 0);

		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		char msg[50] = "";
		sprintf(msg, "Content-Length: %d\n\n", size);
		send(client, msg, strlen(msg), 0);
		//// Send content
		fseek(file, 0, SEEK_SET);
		char line[512];
		while (fgets(line, sizeof(line), file) != NULL) {
			// Replace \n => \r\n
			int i = 0;
			while (line[i] != '\0')
			{
				if (line[i] == '\n')
				{
					line[i] = '\r';
					line[i + 1] = '\n';
					line[i + 2] = '\0';
					i++;
				}
				i++;
			}
			send(client, line, strlen(line), 0);
		}
		fclose(file);
		return;
	}
}
void POST_METHOD(SOCKET client, char * request)
{
	printf("SOCKET %d : POST method\n", client);
	// Log
	char msg[50];
	sprintf(msg, "SOCKET %d : POST method!", client);
	Log(msg, "INFO");
}
void PUT_METHOD(SOCKET client, char * request)
{
	printf("SOCKET %d : PUT method\n", client);
	// Log
	char msg[50];
	sprintf(msg, "SOCKET %d : PUT method!", client);
	Log(msg, "INFO");
}
void HEAD_METHOD(SOCKET client, char * request)
{
	printf("SOCKET %d : HEAD method\n", client);
	// Log
	char msg[50];
	sprintf(msg, "SOCKET %d : HEAD method!", client);
	Log(msg, "INFO");
}
void DELETE_METHOD(SOCKET client, char * request)
{
	printf("SOCKET %d : DELETE method\n", client);
	// Log
	char msg[50];
	sprintf(msg, "SOCKET %d : DELETE method!", client);
	Log(msg, "INFO");
}
void UNKNOWN_METHOD(SOCKET client, char * request)
{
	printf("SOCKET %d : UNKNOWN method\n", client);
	// Log
	char msg[50];
	sprintf(msg, "SOCKET %d : UNKNOWN method!", client);
	Log(msg, "INFO");
}

void SEND_404(SOCKET client)
{
	send(client, RESPONSE_NOT_FOUND, strlen(RESPONSE_NOT_FOUND), 0);
}

void Log(char * msg, char * level)
{
	// Open file
	FILE *file = fopen("HTTPServerLog.txt", "a");
	if (file == NULL) {
		printf("Error open HTTPServerLog.txt!\n");
		return;
	}

	// Get current time
	char buf[32];
	time_t t = time(NULL);
	struct tm *lt = localtime(&t);
	buf[strftime(buf, sizeof(buf), "%Y-%d-%m %H:%M:%S", lt)] = '\0';

	// Contruct Log
	char buffer[1024];
	sprintf(buffer, "%s %s %s\n", buf, level, msg);
	
	// Write to log
	fputs(buffer, file);

	// Close file
	fclose(file);
}