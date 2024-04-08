#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * TODO do parser right
 * line order
 * line numbers
 * => suck
 * strcmp!!
 */

int parseRequestLine(char *requestLine, char ***current_line,
                     http_request *parsedResponse);
int parseRequestHeaders(char *requestLine, char ***current_line,
                        http_request *parsedResponse);
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

  // separate headers from body
  char *separator = strstr(request, "\r\n\r\n");
  if (separator == NULL) {
    printf("No separator!!");
    return NULL;
  }

  char *headers = request;
  size_t headerLen = separator - headers;
  char *header = malloc(headerLen + 1);
  if (!header) {
    printf("malloc header buf failed");
    free(parsedResponse);
    return NULL;
  }
  header = memcpy(header, headers, headerLen);
  header[headerLen] = '\0';

  char *body = separator + 4;

  char **currentLine;
  printf("Headers to parse:\n%s\n", header);
  printf("--------------------------------------\n");
  int ret = parseRequestLine(header, &currentLine, parsedResponse);
  printf("--------------------------------------\n");
  if (ret < 0) {
    fprintf(stderr, "failed to parse requestLine\n");
    free(parsedResponse);
    return NULL;
  }

  printf("parsig requestHeaders\n");
  ret = parseRequestHeaders(header, &currentLine, parsedResponse);
  printf("--------------------------------------\n");
  if (ret < 0) {
    fprintf(stderr, "failed to parse headers\n");
    free(parsedResponse);
    return NULL;
  }
  if (parsedResponse->bodyLen > 0) {
    parsedResponse->body = malloc(parsedResponse->bodyLen);
    if (!parsedResponse->body) {
      perror("failed to malloc bodyBuf");
      free(parsedResponse);
      return NULL;
    }
    void *rret = memcpy(parsedResponse->body, body, parsedResponse->bodyLen);
    if (!rret) {
      perror("failed to memcpy request body");
      free(parsedResponse);
      return NULL;
    }
  }

  printf("returning requeest struct\n");
  return parsedResponse;
}

int parseRequestLine(char *request, char ***current_line,
                     http_request *parsedResponse) {

  // printf("parseRequestlineGot: %s", request);
  if (request == NULL || current_line == NULL) {
    fprintf(stderr, "parseRequestLine: empty input\n");
  }

  // printf("tokenixing request by lines\n");
  char *requestLine = strtok_r(request, "\r\n", *current_line);
  // printf("parsing equest line : %s\n", requestLine);
  // printf("trsdsadljksajdlksajdlksaj\n\n\n\n\n");
  if (requestLine == NULL) {
    perror("request line is NULL");
    return -1;
  }

  printf("\nRequest-Line: %s\n", requestLine);
  char *method = strtok(requestLine, " ");
  if (strcmp(method, "GET") == 0) {
    printf("Got HTTP GET request\n");
    parsedResponse->method = GET;

  } else if (strcmp(method, "POST") == 0) {
    printf("HTTP POST request\n");
    parsedResponse->method = POST;

  } else {
    fprintf(stderr, "unimplemented HTTP method %s\n", method);
    return -1;
  }

  char *path = strtok(NULL, " ");
  if (path == NULL) {
    fprintf(stderr, "failed to parse path: %s\n", path);
    return -1;
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
    return -1;
  }
  printf("gotversion: %s\n", version);
  char *version_end = strcpy(parsedResponse->http_version, version);
  if (!path_end) {
    perror("FUCK");
    return -1;
  }
  size_t version_len = parsedResponse->http_version - version_end;

  return 0;
}

int parseRequestHeaders(char *requestLine, char ***currentLine,
                        http_request *parsedResponse) {
  char *currentHeader;
  char *key;
  char *value;
  printf("parseheaders: %s\n", requestLine);
  while (1) {
    printf("====\n");
    currentHeader = strtok_r(NULL, "\r\n", *currentLine);
    if (currentHeader == NULL) {
      printf("done parsing headers\n");
      break;
    }
    printf("+++++currentHeader: %s\n", currentHeader);
    key = strtok(currentHeader, " ");
    value = strtok(NULL, " ");
    printf("KEY: %s    Value: %s\n", key, value);
    if (key == NULL || strlen(key) == 0) {
      perror("failed to parse header(key)");
      return -1;
    }
    if (value == NULL) {
      perror("failed to parse header(value)");
      return -1;
    }
    if (strcmp(key, "Accept:") == 0) {
      parsedResponse->headers[Accept] = value;
    } else if (strcmp(key, "Accept-Charset:") == 0) {
      parsedResponse->headers[AcceptCharset] = value;
    } else if (strcmp(key, "Accept-Encoding:") == 0) {
      parsedResponse->headers[AcceptEncoding] = value;
    } else if (strcmp(key, "Accept-Language:") == 0) {
      parsedResponse->headers[AcceptLanguage] = value;
    } else if (strcmp(key, "Authorisation:") == 0) {
      parsedResponse->headers[Authorisation] = value;
    } else if (strcmp(key, "Expect: ") == 0) {
      parsedResponse->headers[Expect] = value;
    } else if (strcmp(key, "From:") == 0) {
      parsedResponse->headers[From] = value;
    } else if (strcmp(key, "Host:") == 0) {
      parsedResponse->headers[Host] = value;
    } else if (strcmp(key, "If-Match:") == 0) {
      parsedResponse->headers[IfMatch] = value;
    } else if (strcmp(key, "If-Modified-Since:") == 0) {
      parsedResponse->headers[IfModifiedSince] = value;
    } else if (strcmp(key, "If-None-Match:") == 0) {
      parsedResponse->headers[IfNoneMatch] = value;
    } else if (strcmp(key, "If-Range:") == 0) {
      parsedResponse->headers[IfRange] = value;
    } else if (strcmp(key, "If-Unmodified-Since:") == 0) {
      parsedResponse->headers[IfUnmodifiedSince] = value;
    } else if (strcmp(key, "Max-Forwards:") == 0) {
      parsedResponse->headers[MaxForwards] = value;
    } else if (strcmp(key, "Proxy-Authorisation:") == 0) {
      parsedResponse->headers[ProxyAuthorisation] = value;
    } else if (strcmp(key, "Range:") == 0) {
      parsedResponse->headers[Range] = value;
    } else if (strcmp(key, "Referer:") == 0) {
      parsedResponse->headers[Referer] = value;
    } else if (strcmp(key, "TE:") == 0) {
      parsedResponse->headers[TE] = value;
    } else if (strcmp(key, "Allow:") == 0) {
      parsedResponse->headers[Allow] = value;
    } else if (strcmp(key, "Contetn-Encoding:") == 0) {
      parsedResponse->headers[ContentEncoding] = value;
    } else if (strcmp(key, "Content-Language:") == 0) {
      parsedResponse->headers[ContentLanguage] = value;
    } else if (strcmp(key, "Content-Length:") == 0) {
      parsedResponse->headers[ContentLength] = value;
      parsedResponse->bodyLen = atoi(value);
      if (parsedResponse->bodyLen == 0) {
        perror("failed to parse body len\n");
        return -1;
      }
    } else if (strcmp(key, "Content-Location:") == 0) {
      parsedResponse->headers[ContentLocation] = value;
    } else if (strcmp(key, "User-Agent:") == 0) {
      parsedResponse->headers[UserAgent] = value;
    } else if (strcmp(key, "Content-MD5:") == 0) {
      parsedResponse->headers[ContentMD5] = value;
    } else if (strcmp(key, "Content-Range:") == 0) {
      parsedResponse->headers[ContentRange] = value;
    } else if (strcmp(key, "Content-Type:") == 0) {
      parsedResponse->headers[contentType] = value;
    } else if (strcmp(key, "Expires:") == 0) {
      parsedResponse->headers[Expires] = value;
    } else if (strcmp(key, "extension-header:") == 0) {
      parsedResponse->headers[extensionHeader] = value;
    } else if (strcmp(key, "Last-Modifiedelse :") == 0) {
      parsedResponse->headers[LastModified] = value;
    } else {
      fprintf(stderr, "unknown header: %s %s\n", key, value);
      return -1;
    }
  }

  return 0;
}
