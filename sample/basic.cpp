#include <fcntl.h>
#include <unistd.h>

namespace some_namespace {
void io_fn(const char* pathname) {
  char buf[1024];
  int fd = open(pathname, O_RDONLY);
  int rc = read(fd, buf, 0);
  close(fd);
}
int static_fn(int a, int b) {
  io_fn("/dev/null");
  return a + b;
}
inline int inline_fn() { return static_fn(1, -1); }
}  // namespace some_namespace

int main() { return some_namespace::inline_fn(); }
