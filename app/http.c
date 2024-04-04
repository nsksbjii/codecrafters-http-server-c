#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define MAX_PATH_SIZE 512
// #define MAX_VERSION_SIZE 32

http_request *parseRequest(char *request) {
  if (request == NULL) {
    fprintf(stderr, "cannot parse NULL pointer!\n");
    return NULL;
  }

  // reserve memory
  http_request *parsedResponse = malloc(sizeof(http_request));

  if (parsedResponse == NULL) {
    perror("requestinit failed");
    return NULL;
  }
  char *line1 = strtok(request, "\r\n"); // header
  char *line2 = strtok(NULL, "\r\n");    // host
  char *line3 = strtok(NULL, "\r\n");    // user-agent
  char *line4 = strtok(NULL, "\r\n");    // content-type
  char *line5 = strtok(NULL, "\r\n");    // content-len
  strtok(NULL, "\r\n");                  // content-len
  // strtok(NULL, "\r\n");                  // content-len
  char *body = strtok(NULL, "\r\n"); // content-len

  printf("requestlines:\n");
  printf("line1: %s\n", line1);
  printf("line2: %s\n", line2);
  printf("line3: %s\n", line3);

  // parse first line of request GET / HTTPV1.1
  char *method = strtok(line1, " ");
  if (strcmp(method, "GET") == 0) {
    printf("Got HTTP GET request\n");
    parsedResponse->method = GET;

  } else if (strcmp(method, "POST") == 0) {

    printf("HTTP POST request\n");
    parsedResponse->method = POST;

  } else {

    fprintf(stderr, "unknown HTTP method %s\n", method);
    free(parsedResponse);
    return NULL;
  }

  char *path = strtok(NULL, " ");
  if (path == NULL) {
    fprintf(stderr, "failed to parse path: %s\n", path);
    free(parsedResponse);
    return NULL;
  }
  printf("gotPath: %s\n", path);
  char *path_end = strcpy(parsedResponse->path, path);
  if (!path_end) {
    perror("FUCK");
  }
  size_t path_len = parsedResponse->path - path_end;

  char *version = strtok(NULL, " ");
  if (version == NULL) {
    fprintf(stderr, "failed to parse version: %s\n", version);
    free(parsedResponse);
    return NULL;
  }
  printf("gotversion: %s\n", version);
  char *version_end = strcpy(parsedResponse->http_version, version);
  if (!path_end) {
    perror("FUCK");
    return NULL;
  }
  size_t version_len = parsedResponse->http_version - version_end;

  // parse host line
  if (line2) {
    char *host_end = strcpy(parsedResponse->host, (line2 + 6));
    if (!host_end) {
      perror("FUCK");
      free(parsedResponse);
      return NULL;
    }
    size_t host_len = parsedResponse->host - host_end;
    printf("gothost: %s\n", parsedResponse->host);
  }

  if (line3) {
    // parse user agent line
    char *user_end = strcpy(parsedResponse->user_agent, (line3 + 12));
    if (!user_end) {
      perror("FUCK");
      free(parsedResponse);
      return NULL;
    }
    size_t user_len = parsedResponse->user_agent - user_end;
  }

  if (line5) {
    // content-type
    // fuck this
    printf("line5: %s\n", line4);
  }

  if (line4) {
    // ocntent len
    printf("leni4: %s\n", line5);

    char *tok1 = strtok(line5, " ");
    char *contentLenStr = strtok(NULL, " ");
    parsedResponse->bodyLen = atoi(contentLenStr);
    if (parsedResponse->bodyLen <= 0) {
      fprintf(stderr, "failed to parse contentLen\n");
      free(parsedResponse);
      return NULL;
    }
    if (!contentLenStr) {
      perror("failed to tokenize rewuenst");
    }

    printf("%d\n", parsedResponse->bodyLen);
    parsedResponse->body = malloc(parsedResponse->bodyLen);
    if (!parsedResponse->body) {
      perror("failed to malloc bodyBuf");
      free(parsedResponse);
      return NULL;
    }
    printf("Body:\n%s\n", (body == NULL) ? "NULL" : body);
    void *ret = memcpy(parsedResponse->body, body, parsedResponse->bodyLen);
    if (!ret) {
      perror("failed to memcpy request body");
      free(parsedResponse);
      return NULL;
    }
  }

  printf("returning requeest struct\n");
  return parsedResponse;
}
