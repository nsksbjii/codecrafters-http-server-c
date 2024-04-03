#include "server.h"
#include <stdio.h>

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("useage:\n %s --directory PATH\n", argv[0]);
    server("/home/ketrptr/code/codecrafters/httpServerC/"
           "codecrafters-http-server-c/");
    return 0;
  }

  server(argv[2]);
  return 0;
}
