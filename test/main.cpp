#include "trace.h"

#include "common.h"
#include <cstdio>

void hello_shared_lib();
void hello_static_lib();

GEN_FN(hello)

int main(int argc, char **argv) {
  hello();
  hello_shared_lib();
  hello_static_lib();
}
