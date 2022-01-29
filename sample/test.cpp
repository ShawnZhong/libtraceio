#include <fcntl.h>
#include <unistd.h>

#include <cstdio>

#include "test_common.h"

void hello_shared_lib();
void hello_static_lib();

GEN_FN(hello_main_prog)

void test_lib() {
  hello_main_prog();
  hello_shared_lib();
  hello_static_lib();
}

void test_io() {
  int fd = open("/dev/null", O_RDONLY);
  read(fd, nullptr, 0);
  pread(fd, nullptr, 0, 0);
  write(fd, nullptr, 0);
  pwrite(fd, nullptr, 0, 0);
  close(fd);
}

void test_io_fortify() {
  // When __USE_FORTIFY_LEVEL > 2, some of these calls are replaced with calls
  // to the fortified versions. e.g., read -> __read_chk. See the source code at
  // https://github.com/bminor/glibc/blob/be211e0922faba196d780565875b4617cc9839aa/posix/unistd.h#L1213-L1215
  // https://github.com/bminor/glibc/blob/ae37d06c7d127817ba43850f0f898b793d42aea7/posix/bits/unistd.h#L35-L48

  int fd = open("/dev/null", O_RDONLY);

  {
    char buf[1];
    volatile int size = 0;
    read(fd, buf, size);  // this will be a call to __read_chk
  }

  {
    char buf[1];
    volatile int size = 0;
    pread(fd, buf, size, 0);  // this will be a call to __pread_chk
  }
}

int main() {
  test_lib();
  test_io();
  test_io_fortify();
  return 0;
}
