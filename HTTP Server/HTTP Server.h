#include "stdafx.h"

#define SERVER_PORT 80
#define SERVER_ADDR "127.0.0.1"
#define RESPONSE_STATUS_LINE "HTTP/1.1 200 OK\r\n"
#define RESPONSE_CONTENT_TYPE "Content-Type: text/html; charset = utf-8\r\n"
#define RESPONSE_NOT_FOUND "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-length: 35\r\n\n<html><body>Not Found</body></html>"

// Function
DWORD WINAPI ClientThread(LPVOID lpParams);
void GET_METHOD(SOCKET client, char * request);
void POST_METHOD(SOCKET client, char * request);
void PUT_METHOD(SOCKET client, char * request);
void HEAD_METHOD(SOCKET client, char * request);
void DELETE_METHOD(SOCKET client, char * request);
void UNKNOWN_METHOD(SOCKET client, char * request);
void SEND_404(SOCKET client);
void Log(char * msg, char * level);
