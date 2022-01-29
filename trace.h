#pragma once

#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <fcntl.h>
#include <unistd.h>

#include <cassert>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#define NO_INSTRUMENT __attribute__((no_instrument_function))

namespace func_trace {
constexpr static int indent = 1;

int nspace = 1;

NO_INSTRUMENT void print_fn_name(void *addr) {
  Dl_info dlinfo{};
  auto rc = dladdr(addr, &dlinfo);
  if (rc == 0) {
    fprintf(stderr, "%p\n", addr);
    return;
  }
  if (dlinfo.dli_sname == nullptr) {
    auto rel_diff = reinterpret_cast<uintptr_t>(addr) -
                    reinterpret_cast<uintptr_t>(dlinfo.dli_fbase);
    fprintf(stderr, "%#lx in %s\n", rel_diff, dlinfo.dli_fname);
    return;
  }
  auto name = abi::__cxa_demangle(dlinfo.dli_sname, nullptr, nullptr, nullptr);
  if (name == nullptr) {
    fprintf(stderr, "%s\n", dlinfo.dli_sname);
    return;
  }
  fprintf(stderr, "%s\n", name);
  free(name);
}

NO_INSTRUMENT void print_backtrace() {
  constexpr auto BACKTRACE_SIZE = 128;
  void *callstack[BACKTRACE_SIZE]{};
  int nptrs = backtrace(callstack, BACKTRACE_SIZE);
  fprintf(stderr, "%*s backtraces: \n", nspace, "=");
  for (int i = 1; i < nptrs; i++) {
    fprintf(stderr, "%*s [%d] ", nspace, "=", nptrs - i);
    print_fn_name(callstack[i]);
  }
}

namespace os {
#define DEFINE_FN(name) \
  auto name = reinterpret_cast<decltype(::name) *>(dlsym(RTLD_NEXT, #name))
DEFINE_FN(open);
DEFINE_FN(close);
DEFINE_FN(read);
DEFINE_FN(write);
#undef DEFINE_FN
}  // namespace os

extern "C" {

#define LOG_FN(msg, ...)                                          \
  do {                                                            \
    fprintf(stderr, "%*s " msg "\n", nspace, "=", ##__VA_ARGS__); \
    print_backtrace();                                            \
  } while (0)

int open(const char *pathname, int flags, ...) {
  mode_t mode = 0;
  if (__OPEN_NEEDS_MODE(flags)) {
    va_list arg;
    va_start(arg, flags);
    mode = va_arg(arg, mode_t);
    va_end(arg);
  }
  auto res = os::open(pathname, flags, mode);
  LOG_FN("open(%s, %d, %d) = %d", pathname, flags, mode, res);
  return res;
}

int close(int fd) {
  auto res = os::close(fd);
  LOG_FN("close(%d) = %d", fd, res);
  return res;
}

ssize_t read(int fd, void *buf, size_t count) {
  auto res = os::read(fd, buf, count);
  LOG_FN("read(%d, %p, %zu) = %zd", fd, buf, count, res);
  return res;
}

ssize_t write(int fd, const void *buf, size_t count) {
  auto res = os::write(fd, buf, count);
  LOG_FN("write(%d, %p, %zu) = %zd", fd, buf, count, res);
  return res;
}

#undef LOG_FN

NO_INSTRUMENT void __cyg_profile_func_enter(void *this_fn, void *call_site) {
  fprintf(stderr, "%*s ", nspace, ">");
  print_fn_name(this_fn);
  nspace += indent;
}

NO_INSTRUMENT void __cyg_profile_func_exit(void *this_fn, void *call_site) {
  nspace -= indent;
  fprintf(stderr, "%*s ", nspace, "<");
  print_fn_name(this_fn);
}
}
}  // namespace func_trace
