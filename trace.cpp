#include "trace.h"

#define __USE_FORTIFY_LEVEL 1

#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>

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
    fprintf(stderr, "%s(...)\n", dlinfo.dli_sname);
    return;
  }
  fprintf(stderr, "%s\n", name);
  free(name);
}

NO_INSTRUMENT void print_backtrace() {
  constexpr auto BACKTRACE_SIZE = 128;
  void *callstack[BACKTRACE_SIZE]{};
  int nptrs = backtrace(callstack, BACKTRACE_SIZE);
  nptrs -= 2;  // skip __libc_start_main and _start
  fprintf(stderr, "%*s backtraces: \n", nspace, "=");
  for (int i = 2; i < nptrs; i++) {
    fprintf(stderr, "%*s [%d] ", nspace, "=", nptrs - i);
    print_fn_name(callstack[i]);
  }
}

template <typename Arg, typename... Args>
NO_INSTRUMENT void print(std::ostream &out, Arg &&arg, Args &&...args) {
  out << arg;
  ((out << ", " << args), ...);
}

extern "C" {
#define CALL(name, ...)                                                \
  do {                                                                 \
    static auto ptr = dlsym(RTLD_NEXT, #name);                         \
    auto res = reinterpret_cast<decltype(::name) *>(ptr)(__VA_ARGS__); \
    fprintf(stderr, "%*s " #name "\n", nspace, ">");                   \
    nspace += indent;                                                  \
                                                                       \
    fprintf(stderr, "%*s " #name "(", nspace, "=");                    \
    print(std::cerr, __VA_ARGS__);                                     \
    std::cerr << ") = " << res << "\n";                                \
    print_backtrace();                                                 \
                                                                       \
    nspace -= indent;                                                  \
    fprintf(stderr, "%*s " #name "\n", nspace, "<");                   \
    return res;                                                        \
  } while (0)

NO_INSTRUMENT int open(const char *pathname, int flags, ...) {
  mode_t mode = 0;
  if (__OPEN_NEEDS_MODE(flags)) {
    va_list arg;
    va_start(arg, flags);
    mode = va_arg(arg, mode_t);
    va_end(arg);
  }
  CALL(open, pathname, flags, mode);
}
NO_INSTRUMENT int close(int fd) { CALL(close, fd); }
NO_INSTRUMENT ssize_t read(int fd, void *buf, size_t count) {
  CALL(read, fd, buf, count);
}
NO_INSTRUMENT ssize_t __read_chk(int fd, void *buf, size_t count,
                                 size_t buflen) {
  CALL(__read_chk, fd, buf, count, buflen);
}
NO_INSTRUMENT ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
  CALL(pread, fd, buf, count, offset);
}
NO_INSTRUMENT ssize_t __pread_chk(int fd, void *buf, size_t nbytes,
                                  off_t offset, size_t bufsize) {
  CALL(__pread_chk, fd, buf, nbytes, offset, bufsize);
}
NO_INSTRUMENT ssize_t write(int fd, const void *buf, size_t count) {
  CALL(write, fd, buf, count);
}
NO_INSTRUMENT ssize_t pwrite(int fd, const void *buf, size_t count,
                             off_t offset) {
  CALL(pwrite, fd, buf, count, offset);
}

#undef CALL

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
#undef NO_INSTRUMENT
}  // namespace func_trace
