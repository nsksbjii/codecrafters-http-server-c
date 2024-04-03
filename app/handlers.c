#include "handlers.h"
#include "http.h"
#include <complex.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define REPONSE_BUF_SZ 2048
#define MAX_PATH_SIZE 512
#define DEBUG

int sendFile(FILE *file, int sock);

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

int userAgentHandler(char *user_agent, int sock) {
  if (user_agent == NULL) {
    fprintf(stderr, "userAgentHandler: user_agent is NULL!\n");
    return -1;
  }
  printf("userAgentHandler:choing %s\n", user_agent);

  int bodyLen = strlen(user_agent);
  printf("echo str is %d long\n", bodyLen);
  if (bodyLen < 0) {
    fprintf(stderr, "userAgentHandler: echo len is 0!\n");
    return -1;
  }

  char reposeBuf[REPONSE_BUF_SZ];
  int respSize = sprintf(reposeBuf,
                         "HTTP/1.1 200 OK\r\nContent-Type: "
                         "text/plain\r\nContent-Length: %d\r\n\r\n%s\r\n",
                         bodyLen, user_agent);
  if (respSize < 0) {
    fprintf(stderr, "userAgentHandler: failed to construct response!\n");
    return -1;
  }
  int ret = write(sock, reposeBuf, respSize);
  if (ret != respSize) {
    perror("write to socket failed");
    return -1;
  }
  return ret;
  return 0;
}

int getFileHandler(char *directory, char *filename, int sock) {

  if (directory == NULL) {
    fprintf(stderr, "getFileHandler: directory is NULL!\n");
    return -1;
  }
  if (filename == NULL) {
    fprintf(stderr, "getFileHandler: filename is NULL!\n");
    return -1;
  }
  if ((strlen(directory) + strlen(filename)) > MAX_PATH_SIZE) {
    fprintf(stderr, "path to long!\n");
    return -1;
  }

  printf("dir: %s\n", directory);
  printf("file: %s\n", filename);
  printf("dir: %s\n", directory);

  // open directory
  // DIR *working_dir = opendir(directory);
  // if (!working_dir) {
  //   perror("open dir failed");
  //   return -1;
  // }
  //
  // struct dirent *currentFile;
  // while ((currentFile = readdir(working_dir)) != NULL) {
  //   if (strcmp(currentFile->d_name, filename) == 0) {
  //     sendFile(currentFile, sock);
  //     return 0;
  //   }
  // }
  char pathBuf[MAX_PATH_SIZE];
  char *path = strcat(pathBuf, directory);
  path = strcat(pathBuf, filename);
  path[strlen(path) - 1] = '\0';
  printf("trying to open file: %s\n", path);

  FILE *file = fopen(path, "r");

  if (!file) {
    printf("filenot found sinding 404\n");
    if (write(sock, HTTP_404, sizeof(HTTP_404) < sizeof(HTTP_404))) {
      perror("getFile: failed to send 404");
      return -1;
    }

  } else {
    printf("found file.. tryinng to send it\n");
    sendFile(file, sock);
  }

  /*
   * If <filename> exists in <directory>, you'll need to respond with a 200 OK
   * response. The response should have a content type of
   * application/octet-stream, and it should contain the contents of the file
   * as the body. If the file doesn't exist, return a 404.
   */
  return 0;
}

int sendFile(FILE *file, int sock) {
  char *fileBuf;
  size_t fileBufLen;

  // Get file length
  fseek(file, 0, SEEK_END);
  fileBufLen = ftell(file);
  fseek(file, 0, SEEK_SET);

  printf("file is %zu bytes long\n", fileBufLen);

  // Allocate memory
  fileBuf = (char *)malloc(fileBufLen + 1);
  if (!fileBuf) {
    perror("failed to allocate fileBuf");
    fclose(file);
    return 0;
  }

  // Read file contents into buf
  int read = fread(fileBuf, 1, fileBufLen, file);

  printf("read %d bytes form file\n", read);
  fclose(file);
  if (read != fileBufLen) {
    fprintf(stderr, "read != fileLen!!\n");
    free(fileBuf);
    return -1;
  }

  // construct response
  int ret;
  char *dest;
  const char *header = HTTP_OK;
  const char *content_typeOctetStream =
      "Content-Type: application/octet-stream\r\n";
  const char *consten_len = "Content-Length: ";

  size_t respBufLen = strlen(consten_len) + strlen(content_typeOctetStream) +
                      strlen(header) + read + 512;
  char *responseBuf = malloc(respBufLen);
  if (!responseBuf) {
    perror("failed to allocate response buffer!\n");
    free(fileBuf);
    return -1;
  }
  memset(responseBuf, 0, respBufLen);

  ret = sprintf(responseBuf, "%s%s%s%d\r\n\r\n%s", header,
                content_typeOctetStream, consten_len, read, fileBuf);
  if (ret < 0) {
    perror("failed to build response string");
    free(fileBuf);
    free(responseBuf);
    return -1;
  }
  printf("responseBuff: \n\n");
  printf("%s", responseBuf);

  int sent = write(sock, responseBuf, ret);
  if (sent != ret) {
    fprintf(stderr, "sent != rerponseLen");
    free(responseBuf);
    free(fileBuf);
    return -1;
  }

  printf("sent %dbytes\n", sent);

  free(responseBuf);
  free(fileBuf);
  return ret;
}