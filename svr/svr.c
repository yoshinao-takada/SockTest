#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "TCPMsgHandler.h"

char* buf;
const SIZE_T BUFSIZE = 1024;
SOCKET s, sc;
struct sockaddr_in addr;
WSADATA wsadata;
int port = 5000;
char clntName[INET_ADDRSTRLEN];

int init_buf()
{
	int err = ERROR_SUCCESS;
	do {
		if ((buf = CoTaskMemAlloc(BUFSIZE)) == NULL)
		{
			err = GetLastError();
		}
	} while (0);
	return err;
}

int main()
{
	int err = ERROR_SUCCESS;
	do {
		if (err = init_buf())
		{
			break;
		}
		if (WSAStartup(0x202, &wsadata))
		{
			err = WSAGetLastError();
			fprintf(stderr, "WSAStartup() failed, err = %d @ %s,%d\n",
				err, __FILE__, __LINE__);
			break;
		}
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(port);

		if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		{
			err = WSAGetLastError();
			fprintf(stderr, "socket() failed, err = %d @ %s,%d\n",
				err, __FILE__, __LINE__);
			break;
		}

		// bind the socket to the local address
		if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		{
			err = WSAGetLastError();
			fprintf(stderr, "bind() failed, err = %d @ %s,%d\n",
				err, __FILE__, __LINE__);
			break;
		}

		// start to listen
		if (listen(s, 16) < 0)
		{
			err = WSAGetLastError();
			fprintf(stderr, "listen() failed, err = %d @ %s,%d\n",
				err, __FILE__, __LINE__);
			break;
		}
		sc = INVALID_SOCKET;
		for (;;)
		{
			struct sockaddr_in clntAddr;
			int clntAddrLen = sizeof(clntAddr);
			sc = accept(s, (struct sockaddr*)&clntAddr, &clntAddrLen);
			if (sc < 0)
			{
				err = WSAGetLastError();
				fprintf(stderr, "accept() failed, err = %d @ %s,%d\n",
					err, __FILE__, __LINE__);
				break;
			}
			
			// print the client socket name
			if (inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr, clntName,
				sizeof(clntName)) != NULL)
			{
				printf("Client %s:%d\n", clntName, ntohs(clntAddr.sin_port));
			}
			else
			{
				err = WSAGetLastError();
				fprintf(stderr, "inet_ntop() failed, err = %d @ %s,%d\n",
					err, __FILE__, __LINE__);
				break;
			}
			err = HandleTCPCli(sc);
			sc = INVALID_SOCKET;
		}
	} while (0);
	if (buf)
	{
		CoTaskMemFree(buf);
		buf = NULL;
	}
	WSACleanup();
	return err;
}