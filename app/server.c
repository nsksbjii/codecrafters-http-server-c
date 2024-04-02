#include "handlers.h"
#include "http.h"
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PACKET_BUF_SZ 4096
#define DEBUG

int handleConnection(int current_sock);
int main() {
  // Disable output buffering
  setbuf(stdout, NULL);

  // You can use print statements as follows for debugging, they'll be visible
  // when running tests.
  printf("Logs from your program will appear here!\n");

  // Uncomment this block to pass the first stage

  unsigned int server_fd, client_addr_len;
  struct sockaddr_in client_addr;

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    printf("Socket creation failed: %s...\n", strerror(errno));
    return 1;
  }

  // Since the tester restarts your program quite often, setting REUSE_PORT
  // ensures that we don't run into 'Address already in use' errors
  unsigned int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) <
      0) {
    printf("SO_REUSEPORT failed: %s \n", strerror(errno));
    return 1;
  }

  struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(4221),
      .sin_addr = {htonl(INADDR_ANY)},
  };

  if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
    printf("Bind failed: %s \n", strerror(errno));
    return 1;
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    printf("Listen failed: %s \n", strerror(errno));
    return 1;
  }
  while (1) {

    printf("Waiting for a client to connect...\n");
    client_addr_len = sizeof(client_addr);

    int current_sock =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (current_sock < 0) {
      perror("accept");
      return 1;
    }
    printf("Client connected\n");

    int pid;
    if ((pid = fork()) == -1) {
      perror("fork");
      return 1;
    } else if (pid == 0) { // child
      close(server_fd);
      handleConnection(current_sock);
      return 0;
    } else {
      close(current_sock);
    }
  }
}
int handleConnection(int current_sock) {
  char recvBuf[PACKET_BUF_SZ];
  int ret;

  ret = read(current_sock, recvBuf, PACKET_BUF_SZ);
  if (ret < 0) {
    perror("read from socket failed");
    return 1;
  }
  printf("%s\n", recvBuf);

  http_request *request = parseRequest(recvBuf);
  if (!request) {
    fprintf(stderr, "Failed to parse request\n");
    return 1;
  }

  // #ifdef DEBUG
  printf("parsed request:\n");
  printf("Method: %s\n", (request->method == GET) ? "GET" : "unknown");
  printf("path: %s\n", request->path);
  printf("host: %s\n", request->host);
  printf("user-agent: %s\n", request->user_agent);
  // #endif

  if (request->method == GET) {
    printf("processing GET request\n");
    if (request->path[0] == '/' && request->path[1] == '\0') {
      printf("sending OK\n");
      printf("%s\n", HTTP_OK);
      ret = write(current_sock, HTTP_OK, sizeof(HTTP_OK) / sizeof(char));
    } else {
      char *path1 = strtok(request->path, "/");
      // printf("%s\n", path1);
      if (strcmp(path1, "echo") == 0) {
        printf("calling echo hadler!\n");
        path1 = strtok(NULL, "/");
        path1[strlen(path1)] = '/';
        echoHandler(path1, current_sock);
      } else if (strcmp(path1, "user-agent") == 0) {

        printf("returning user agent!\n");
        userAgentHandler(request->user_agent, current_sock);

      } else {

        printf("sending 404\n");

        ret = write(current_sock, HTTP_404, sizeof(HTTP_404) / sizeof(char));
      }
    }
    if (ret < 0) {
      perror("writing to socket failed!");
      return 1;
    }
  }

  close(current_sock);
  return 0;
}
