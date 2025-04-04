#include <stdio.h>
#include "TCPMsgHandler.h"
#include "Responder.h"

char *buf = NULL;
SIZE_T BUFSIZE = 0x8000;

int HandleTCPCli(SOCKET sc)
{
	int err = ERROR_SUCCESS;
	int recvlen = 0, sentlen = 0, msglen = 0, responseLen = 0;
	char* response = NULL;
	do {
		if (!(buf = (char*)CoTaskMemAlloc(BUFSIZE)))
		{
			err = GetLastError();
			fprintf(stderr, "CoTaskMemAlloc() failed, err = %d @ %s,%d\n",
				err, __FILE__, __LINE__);
		}
		do {
			recvlen = recv(sc, buf, BUFSIZE, 0);
			if (recvlen == 0)
			{
				fprintf(stdout, "sc was closed by client.\n");
				break;
			}
			else
			{
				err = WSAGetLastError();
				fprintf(stderr, "recv() failed, err = %d @ %s,%d\n",
					err, __FILE__, __LINE__);
				break;
			}
			buf[recvlen] = '\0';
			fprintf(stdout, "request: %s\n", buf);
			if (err = Responder_Respond(buf, recvlen, &response, &responseLen))
			{
				break;
			}
			fprintf(stdout, "response: %s\n", response);
			if ((sentlen = send(sc, response, responseLen, 0)) != responseLen)
			{
				err = WSAGetLastError();
				fprintf(stderr, "send() failed, err = %d @ %s,%d\n",
					err, __FILE__, __LINE__);
				break;
			}
			CoTaskMemFree(response);
		} while (recvlen);
	} while (0);
	if (buf)
	{
		CoTaskMemFree(buf);
		buf = NULL;
	}
	closesocket(sc);
	return err;
}
