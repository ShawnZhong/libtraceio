#include "trace.h"
#include <fcntl.h>
#include <unistd.h>

void test(const char *pathname) {
  int fd = open(pathname, O_RDONLY);
  read(fd, nullptr, 0);
  write(fd, nullptr, 0);
  close(fd);
}

int main(int argc, char **argv) { test("/dev/null"); }
