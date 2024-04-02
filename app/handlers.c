#include "handlers.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define REPONSE_BUF_SZ 2048

int echoHandler(char *toEcho, int sock) {
  if (toEcho == NULL) {
    fprintf(stderr, "echoHandler: toEcho is NULL!\n");
    return -1;
  }
  printf("echoHandler: echoing %s\n", toEcho);

  int bodyLen = strlen(toEcho);
  printf("echo str is %d long\n", bodyLen);
  if (bodyLen < 0) {
    fprintf(stderr, "echoHandler: echo len is 0!\n");
    return -1;
  }

  char reposeBuf[REPONSE_BUF_SZ];
  int respSize = sprintf(reposeBuf,
                         "HTTP/1.1 200 OK\r\nContent-Type: "
                         "text/plain\r\nContent-Length: %d\r\n\r\n%s\r\n",
                         bodyLen, toEcho);
  if (respSize < 0) {
    fprintf(stderr, "echoHandler: failed to construct response!\n");
    return -1;
  }
  int ret = write(sock, reposeBuf, respSize);
  if (ret != respSize) {
    perror("write to socket failed");
    return -1;
  }
  return ret;
}
