#include <string.h>
#include <memory.h>
#include <Windows.h>
#include "Responder.h"

static int cnt = 0;

int Responder_Respond(const char* req, int reqlen, char** res, int* reslen)
{
    const char* FMT[] =
    {
        "[Res %d:",
        "]"
    };
    int err = ERROR_SUCCESS;
    char* buf = NULL;
    size_t bufsize = 0;
    int offset = 0;
    do {
        bufsize = strlen(FMT[0]) + 6 + reqlen + strlen(FMT[1]) + 1;
        buf = (char*)CoTaskMemAlloc(bufsize);
        offset += _snprintf_s(buf, bufsize, bufsize, FMT[0], cnt++);
        memcpy(buf + offset, req, reqlen);
        offset += reqlen;
        offset += _snprintf_s(buf + offset, bufsize - offset, bufsize - offset, FMT[1]);
        *reslen = offset;
        *res = buf;
    } while (0);
    return err;
}
