#ifndef HANDLERS
#define HANDLERS

#include "http.h"
int echoHandler(char *toEcho, int sock);
int userAgentHandler(char *user_agent, int sock);
int getFileHandler(char *directory, char *filename, int sock);

#endif // !HADLERS
#define HANDERLS