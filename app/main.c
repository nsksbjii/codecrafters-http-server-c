#include "server.h"
#include <stdio.h>

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("useage:\n %s --directory PATH\n", argv[0]);
    return 1;
  }

  server(argv[2]);
}
