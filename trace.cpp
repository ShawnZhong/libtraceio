#define _FORTIFY_SOURCE 2
#define __OPTIMIZE__ 1

#include "trace.h"

#include <cxxabi.h>
#include <dirent.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <fcntl.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>

namespace func_trace {
constexpr static int indent = 1;

int nspace = 1;

NO_INSTR void print_fn_name(void *addr) {
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

NO_INSTR void print_backtrace() {
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

template <auto Tag, typename Fn, typename... Args>
NO_INSTR inline auto call(const char *name, Args &&...args) {
  static auto fn_ptr = dlsym(RTLD_NEXT, name);
  auto res = reinterpret_cast<Fn>(fn_ptr)(args...);
  fmt::print(stderr, "{:>{}} {}({}) = {}\n", ">", nspace, name,
             fmt::join(std::make_tuple(args...), ", "), res);
  nspace += indent;
  print_backtrace();
  nspace -= indent;
  fmt::print(stderr, "{:>{}} {}(...)\n", "<", nspace, name);
  return res;
}

extern "C" {
#define CALL(fn, ...) return call<&::fn, decltype(::fn) *>(#fn, __VA_ARGS__)

NO_INSTR int open(const char *pathname, int flags, ...) {
  mode_t mode = 0;
  if (__OPEN_NEEDS_MODE(flags)) {
    va_list arg;
    va_start(arg, flags);
    mode = va_arg(arg, mode_t);
    va_end(arg);
    CALL(open, pathname, flags, mode);
  }
  CALL(open, pathname, flags);
}
NO_INSTR int close(int fd) { CALL(close, fd); }
NO_INSTR ssize_t read(int fd, void *buf, size_t count) {
  CALL(read, fd, buf, count);
}
NO_INSTR ssize_t __read_chk(int fd, void *buf, size_t count, size_t buflen) {
  CALL(__read_chk, fd, buf, count, buflen);
}
NO_INSTR ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
  CALL(pread, fd, buf, count, offset);
}
NO_INSTR ssize_t pread64(int fd, void *buf, size_t count, off64_t offset) {
  CALL(pread64, fd, buf, count, offset);
}
NO_INSTR ssize_t __pread_chk(int fd, void *buf, size_t nbytes, off_t offset,
                             size_t bufsize) {
  CALL(__pread_chk, fd, buf, nbytes, offset, bufsize);
}
NO_INSTR ssize_t __pread64_chk(int fd, void *buf, size_t nbytes, off64_t offset,
                               size_t bufsize) {
  CALL(__pread64_chk, fd, buf, nbytes, offset, bufsize);
}
NO_INSTR ssize_t write(int fd, const void *buf, size_t count) {
  CALL(write, fd, buf, count);
}
NO_INSTR ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
  CALL(pwrite, fd, buf, count, offset);
}
NO_INSTR ssize_t pwrite64(int fd, const void *buf, size_t count,
                          off64_t offset) {
  CALL(pwrite64, fd, buf, count, offset);
}
NO_INSTR off_t lseek(int fd, off_t offset, int whence) {
  CALL(lseek, fd, offset, whence);
}
NO_INSTR off64_t lseek64(int fd, off64_t offset, int whence) {
  CALL(lseek64, fd, offset, whence);
}
NO_INSTR int ftruncate(int fd, off_t length) { CALL(ftruncate, fd, length); }
NO_INSTR int ftruncate64(int fd, off64_t length) {
  CALL(ftruncate64, fd, length);
}

NO_INSTR int fsync(int fd) { CALL(fsync, fd); }
NO_INSTR int fdatasync(int fd) { CALL(fdatasync, fd); }
NO_INSTR int fcntl(int fd, int cmd, ...) {
  void *ptr;
  va_list arg;
  va_start(arg, cmd);
  ptr = va_arg(arg, void *);
  va_end(arg);
  CALL(fcntl, fd, cmd, ptr);
}
NO_INSTR int access(const char *pathname, int mode) {
  CALL(access, pathname, mode);
}
NO_INSTR int unlink(const char *pathname) { CALL(unlink, pathname); }
NO_INSTR int mkdir(const char *pathname, mode_t mode) {
  CALL(mkdir, pathname, mode);
}
NO_INSTR int rmdir(const char *pathname) { CALL(rmdir, pathname); }
NO_INSTR void *mmap(void *addr, size_t length, int prot, int flags, int fd,
                    off_t offset) {
  CALL(mmap, addr, length, prot, flags, fd, offset);
}
NO_INSTR void *mmap64(void *addr, size_t length, int prot, int flags, int fd,
                      off64_t offset) {
  CALL(mmap64, addr, length, prot, flags, fd, offset);
}
NO_INSTR int munmap(void *addr, size_t length) { CALL(munmap, addr, length); }
NO_INSTR int mprotect(void *addr, size_t len, int prot) {
  CALL(mprotect, addr, len, prot);
}
NO_INSTR int msync(void *addr, size_t len, int flags) {
  CALL(msync, addr, len, flags);
}
NO_INSTR int madvise(void *addr, size_t len, int advice) {
  CALL(madvise, addr, len, advice);
}
#undef CALL

NO_INSTR void __cyg_profile_func_enter(void *this_fn, void *call_site) {
  fprintf(stderr, "%*s ", nspace, ">");
  print_fn_name(this_fn);
  nspace += indent;
}

NO_INSTR void __cyg_profile_func_exit(void *this_fn, void *call_site) {
  nspace -= indent;
  fprintf(stderr, "%*s ", nspace, "<");
  print_fn_name(this_fn);
}
}
#undef NO_INSTR
}  // namespace func_trace
