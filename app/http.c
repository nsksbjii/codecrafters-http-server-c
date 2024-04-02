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

  char *method = strtok(request, " ");
  if (strcmp(method, "GET") == 0) {
    printf("Got HTTP GET request\n");
    parsedResponse->method = GET;
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
  if (path == NULL) {
    fprintf(stderr, "failed to parse version: %s\n", version);
    free(parsedResponse);
    return NULL;
  }
  char *version_end = strcpy(parsedResponse->http_version, version);
  if (!path_end) {
    perror("FUCK");
  }
  size_t version_len = parsedResponse->http_version - version_end;

  return parsedResponse;
}
