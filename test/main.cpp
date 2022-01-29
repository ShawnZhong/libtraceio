#include <cstdio>

#include "common.h"
#include "trace.h"

void hello_shared_lib();
void hello_static_lib();

GEN_FN(hello_main_prog)

void hello() {
  hello_main_prog();
  hello_shared_lib();
  hello_static_lib();
}

void io() {
  int fd = open("/dev/null", O_RDONLY);
  read(fd, nullptr, 0);
  write(fd, nullptr, 0);
  close(fd);
}

int main() {
  hello();
  io();
  return 0;
}
