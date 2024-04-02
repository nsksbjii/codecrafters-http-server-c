#ifndef HTTP_RESP
#define HTTP_RESP

/* extern const char HTTP_OK[]; */
/* extern const char HTTP_404[]; */

#define HTTP_OK "HTTP/1.1 200 OK\r\n\r\n"
#define HTTP_404 "HTTP/1.1 404 Not Found\r\n\r\n"

typedef enum {
  GET,
  POST,
  PUT,
} HTTP_METHOD;

typedef struct {
  HTTP_METHOD method;
  char path[512];
  char http_version[256];
} http_request;

http_request *parseRequest(char *request);
void freeRequest(http_request *toFree);

#endif
