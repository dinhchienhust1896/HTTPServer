#include "stdafx.h"

#define SERVER_PORT 9000
#define SERVER_ADDR "127.0.0.1"
#define RESPONSE_STATUS_LINE "HTTP/1.1 200 OK\r\n"
#define RESPONSE_CONTENT_TYPE "Content-Type: text/html; charset = utf-8\r\n"
#define RESPONSE_NOT_FOUND "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-length: 35\r\n\n<html><body>Not Found</body></html>"
#define RESPONSE_DONE "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-length: 30\r\n\n<html><body>Done</body></html>"
#define RESPONSE_POST_DONE "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-length: 45\r\n\n<html><body>Create file success</body></html>"
#define RESPONSE_PUT_DONE "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-length: 45\r\n\n<html><body>Modify file success</body></html>"
// Function
DWORD WINAPI ClientThread(LPVOID lpParams);
void GET_METHOD(SOCKET client, char * uri);
void POST_METHOD(SOCKET client, char * request);
void PUT_METHOD(SOCKET client, char * request);
void HEAD_METHOD(SOCKET client, char * uri);
void DELETE_METHOD(SOCKET client, char * uri);
void UNKNOWN_METHOD(SOCKET client, char * request);
void Log(char * msg, char * level);
