#pragma once

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <fcntl.h>
#include <unistd.h>

namespace func_trace {
int nspace = 1;
constexpr static int indent = 1;

__attribute__((no_instrument_function)) void print_fn_name(void *addr) {
  Dl_info dlinfo{};
  dladdr(addr, &dlinfo);
  if (!dlinfo.dli_sname) {
    fprintf(stderr, "%p\n", addr);
    return;
  }
  auto name = abi::__cxa_demangle(dlinfo.dli_sname, nullptr, nullptr, nullptr);
  if (!name) {
    fprintf(stderr, "%s\n", dlinfo.dli_sname);
    return;
  }
  fprintf(stderr, "%s\n", name);
  free(name);
}

__attribute__((no_instrument_function)) void print_backtrace() {
  constexpr auto BACKTRACE_SIZE = 128;
  void *callstack[BACKTRACE_SIZE]{};
  int nptrs = backtrace(callstack, BACKTRACE_SIZE);

  fprintf(stderr, "%*s backtraces: \n", nspace + indent, "=");
  for (int i = 1; i < nptrs; i++) {
    fprintf(stderr, "%*s [%d] ", nspace + indent, "=", nptrs - i);
    print_fn_name(callstack[i]);
  }
}

auto my_read = reinterpret_cast<decltype(::read) *>(dlsym(RTLD_NEXT, "read"));
auto my_write =
    reinterpret_cast<decltype(::write) *>(dlsym(RTLD_NEXT, "write"));

extern "C" {
ssize_t read(int fd, void *buf, size_t count) {
  auto res = my_read(fd, buf, count);
  fprintf(stderr, "%*s ", nspace + indent, "=");
  fprintf(stderr, "read(%d, %p, %zu) = %ld\n", fd, buf, count, res);
  print_backtrace();
  return res;
}

ssize_t write(int fd, const void *buf, size_t count) {
  auto res = my_write(fd, buf, count);
  fprintf(stderr, "%*s ", nspace + indent, "=");
  fprintf(stderr, "write(%d, %p, %zu) = %ld\n", fd, buf, count, res);
  print_backtrace();
  return res;
}

__attribute__((no_instrument_function)) void
__cyg_profile_func_enter(void *this_fn, void *call_site) {
  fprintf(stderr, "%*s ", nspace, ">");
  print_fn_name(this_fn);
  nspace += indent;
}

__attribute__((no_instrument_function)) void
__cyg_profile_func_exit(void *this_fn, void *call_site) {
  nspace -= indent;
  fprintf(stderr, "%*s ", nspace, "<");
  print_fn_name(this_fn);
}
}
} // namespace func_trace
