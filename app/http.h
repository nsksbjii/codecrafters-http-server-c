#ifndef HTTP_RESP
#define HTTP_RESP

/* extern const char HTTP_OK[]; */
/* extern const char HTTP_404[]; */

#define HTTP_OK "HTTP/1.1 200 OK\r\n\r\n"
#define HTTP_404 "HTTP/1.1 404 Not Found\r\n\r\n"
#define HTTP_405 "HTTP/1.1 405 Method Not Allowed\r\n\r\n"
#define HTTP_501 "HTTP/1.1 501 Not Implemented\r\n\r\n"
#define HTTP_406 "HTTP/1.1 406 Not Acceptable\r\n\r\n"
#define HTTP_201 "HTTP/1.1 201 Created\r\n\r\n"

typedef enum {
  GET,
  POST,
  PUT,
  OPTIONS,
  HEAD,
  DELETE,
  TRACE,
  CONNECT
} HTTP_METHOD;

typedef enum {
  Accept,
  AcceptCharset,
  AcceptEncoding,
  AcceptLanguage,
  Authorisation,
  Expect,
  From,
  Host,
  IfMatch,
  IfModifiedSince,
  IfNoneMatch,
  IfRange,
  IfUnmodifiedSince,
  MaxForwards,
  ProxyAuthorisation,
  Range,
  Referer,
  TE,
  UserAgent,
} RequestHader;

typedef struct {
  HTTP_METHOD method;
  char path[512];
  char http_version[256];
  char host[256];
  char user_agent[1024];
  /* char *content_type; */
  int bodyLen;
  char *body;
  char *headers[sizeof(RequestHader)];
} http_request;

http_request *parseRequest(char *request);
/* void freegequest(http_request *toFree); */

#endif
