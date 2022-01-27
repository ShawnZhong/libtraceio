#pragma once

#include <cstdio>
#include <cstdlib>
#include <cxxabi.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>

extern "C" {
int nspace = 1;
const int indent = 2;

auto my_read = reinterpret_cast<decltype(::read) *>(dlsym(RTLD_NEXT, "read"));
auto my_write =
    reinterpret_cast<decltype(::write) *>(dlsym(RTLD_NEXT, "write"));

ssize_t read(int fd, void *buf, size_t count) {
  auto res = my_read(fd, buf, count);
  printf("%*s ", nspace + indent, "=");
  printf("read(%d, %p, %zu) = %ld\n", fd, buf, count, res);
  return res;
}

ssize_t write(int fd, const void *buf, size_t count) {
  auto res = my_write(fd, buf, count);
  printf("%*s ", nspace + indent, "=");
  printf("write(%d, %p, %zu) = %ld\n", fd, buf, count, res);
  return res;
}

__attribute__((no_instrument_function)) void print_fn_name(void *addr) {
  Dl_info dlinfo{};
  dladdr(addr, &dlinfo);
  int status;
  auto name = abi::__cxa_demangle(dlinfo.dli_sname, nullptr, nullptr, &status);
  if (status == 0 && name) {
    printf("%s\n", name);
    free(name);
  } else {
    printf("%s\n", dlinfo.dli_sname);
  }
}

__attribute__((no_instrument_function)) void
__cyg_profile_func_enter(void *this_fn, void *call_site) {
  printf("%*s ", nspace, ">");
  print_fn_name(this_fn);
  nspace += indent;
}

__attribute__((no_instrument_function)) void
__cyg_profile_func_exit(void *this_fn, void *call_site) {
  nspace -= indent;
  printf("%*s ", nspace, "<");
  print_fn_name(this_fn);
}
}
