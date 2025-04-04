#pragma once
#ifdef __cplusplus
extern "C" {
#endif
	typedef int (*RESPONDER)(const char* req, int reqlen, char** res, int* reslen);

	int Responder_Respond(const char* req, int reqlen, char** res, int* reslen);
#ifdef __cplusplus
}
#endif