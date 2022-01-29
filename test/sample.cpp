#include <fcntl.h>
#include <unistd.h>

void test_io(const char* pathname) {
  int fd = open(pathname, O_RDONLY);
  read(fd, nullptr, 0);
  close(fd);
}

int main() { test_io("/dev/null"); }
