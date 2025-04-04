#include <WinSock2.h>
#include <stdio.h>
#include <memory.h>

SOCKET s;
struct sockaddr_in addr;
WSADATA wsadata;
int port = 5000;
const char* localhost = "127.0.0.1";
const int ITER = 8;
const char* FMT = "Client string iteration %d";
char* buf = NULL;
SIZE_T bufsize = 0;

int init_buf()
{
	int err = ERROR_SUCCESS;
	do {
		bufsize = 4 * strlen(FMT);
		if (NULL == (buf = CoTaskMemAlloc(bufsize)))
		{
			err = GetLastError();
			fprintf(stderr, "CoTaskMemAlloc() failed, err = %d @ %s,%d\n",
				err, __FILE__, __LINE__);
		}
	} while (0);
	return err;
}

int main()
{
	int err = ERROR_SUCCESS;
	int sentlen, recvlen, msglen;
	do {
		if (err = ini_buf())
		{
			break;
		}
		if (err = WSAStartup(0x202, &wsadata))
		{
			fprintf(stderr, "WSAStartup() failed, err = %d @ %s,%d\n",
				err, __FILE__, __LINE__);
			break;
		}
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		if (inet_pton(AF_INET, localhost, &addr.sin_addr.s_addr) <= 0)
		{
			err = WSAGetLastError();
			fprintf(stderr, "inet_pton() failed, err = %d @ %s,%d\n",
				err, __FILE__, __LINE__);
			break;
		}
		addr.sin_port = htons(port);

		if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		{
			err = WSAGetLastError();
			fprintf(stderr, "socket() failed, err = %d @ %s,%d\n",
				err, __FILE__, __LINE__);
			break;
		}
		if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		{
			err = WSAGetLastError();
			fprintf(stderr, "connect() failed, err = %d @ %s,%d\n",
				err, __FILE__, __LINE__);
			break;
		}
		for (int i = 0; i < ITER; i++)
		{
			msglen = _snprintf_s(buf, bufsize, bufsize, FMT, i) + 1;
			sentlen = send(s, buf, msglen, 0);
			if (sentlen != msglen)
			{
				err = WSAGetLastError();
				fprintf(stderr, "send() failed, err = %d @ %s,%d\n",
					err, __FILE__, __LINE__);
				break;
			}
			Sleep(10);
			recvlen = recv(s, buf, bufsize, 0);
			if (recvlen <= 0)
			{
				err = WSAGetLastError();
				fprintf(stderr, "recv() failed, err = %d @ %s,%d\n",
					err, __FILE__, __LINE__);
				break;
			}
			buf[recvlen] = '\0';
			fprintf(stdout, "recv: %s\n", buf);
		}
		closesocket(s);
	} while (0);
	WSACleanup();
	if (buf)
	{
		CoTaskMemFree(buf);
		buf = NULL;
	}
	return err;
}