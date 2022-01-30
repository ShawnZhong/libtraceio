#define _FORTIFY_SOURCE 2
#define __OPTIMIZE__ 1

#include "calltrace.h"

#include <cxxabi.h>
#include <dirent.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <fcntl.h>
#include <fmt/core.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <chrono>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

namespace calltrace {
namespace config {
static int indent = 1;
static bool print_io_backtrace = true;
static bool print_fn_trace = true;
}  // namespace config

__attribute__((constructor)) void ctor() {
  if (auto env = getenv("TRACE_INDENT"); env != nullptr)
    config::indent = std::stoi(env);
  if (auto env = getenv("TRACE_PRINT_IO_BACKTRACE"); env != nullptr)
    config::print_io_backtrace = env[0] == '1';
  if (auto env = getenv("TRACE_PRINT_FN_TRACE"); env != nullptr)
    config::print_fn_trace = env[0] == '1';
}

std::vector<void *> call_stack;

static int nspace = 1;

static void print_fn_name(void *addr) {
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

static void print_backtrace() {
  if (!config::print_io_backtrace) return;
  fmt::print(stderr, "{:>{}} backtraces:\n", "=", nspace);
  for (int i = call_stack.size() - 1; i >= 0; --i) {
    fmt::print(stderr, "{:>{}} [{}] ", "=", nspace, i);
    print_fn_name(call_stack[i]);
  }
}

static void print_enter_trace(void *addr) {
  call_stack.emplace_back(addr);
  if (!config::print_fn_trace) return;
  fmt::print(stderr, "{:>{}} ", ">", nspace);
  print_fn_name(addr);
  nspace += config::indent;
}

static void print_exit_trace(void *addr) {
  call_stack.pop_back();
  if (!config::print_fn_trace) return;
  nspace -= config::indent;
  fmt::print(stderr, "{:>{}} ", "<", nspace);
  print_fn_name(addr);
}

std::ostream &operator<<(std::ostream &os, const struct stat *s) {
  if (s == nullptr) return os << "nullptr";
  os << fmt::format(
      "{{dev={}, ino={}, mode={}, nlink={}, uid={}, gid={}, rdev={}, size={}, "
      "blksize={}, blocks={}, atim={}.{}, mtim={}.{}, ctim={}.{}}}",
      s->st_dev, s->st_ino, s->st_mode, s->st_nlink, s->st_uid, s->st_gid,
      s->st_rdev, s->st_size, s->st_blksize, s->st_blocks, s->st_atim.tv_sec,
      s->st_atim.tv_nsec, s->st_mtim.tv_sec, s->st_mtim.tv_nsec,
      s->st_ctim.tv_sec, s->st_ctim.tv_nsec);
  return os;
}

template <class Head, class... Tail>
void print(Head const &head, Tail const &...tail) {
  std::cerr << head;
  if constexpr (sizeof...(tail) > 0) {
    std::cerr << ", ";
    print(tail...);
  }
}

template <auto Fn, typename... Args>
inline static auto call(const char *name, Args &&...args) {
  static void *fn = dlsym(RTLD_NEXT, name);
  auto ts = std::chrono::high_resolution_clock::now();
  auto res = reinterpret_cast<decltype(Fn)>(fn)(std::forward<Args>(args)...);
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::high_resolution_clock::now() - ts);
  fmt::print(stderr, "{:>{}} {}(", ">", nspace, name);
  print(std::forward<Args>(args)...);
  std::cerr << ") = " << res << " in " << duration.count() << " ns\n";
  nspace += config::indent;
  print_backtrace();
  nspace -= config::indent;
  fmt::print(stderr, "{:>{}} {}(...)\n", "<", nspace, name);
  return res;
}
#define CALL(fn, ...) return call<::fn>(#fn, __VA_ARGS__)

extern "C" {
int open(const char *path, int flags, ...) {
  mode_t mode = 0;
  if (__OPEN_NEEDS_MODE(flags)) {
    va_list arg;
    va_start(arg, flags);
    mode = va_arg(arg, mode_t);
    va_end(arg);
    CALL(open, path, flags, mode);
  }
  CALL(open, path, flags);
}
int close(int fd) { CALL(close, fd); }
ssize_t read(int fd, void *buf, size_t n) { CALL(read, fd, buf, n); }
ssize_t __read_chk(int fd, void *buf, size_t n, size_t buflen) {
  CALL(__read_chk, fd, buf, n, buflen);
}
ssize_t pread(int fd, void *buf, size_t n, off_t off) {
  CALL(pread, fd, buf, n, off);
}
ssize_t pread64(int fd, void *buf, size_t n, off64_t off) {
  CALL(pread64, fd, buf, n, off);
}
ssize_t __pread_chk(int fd, void *buf, size_t n, off_t off, size_t bufsize) {
  CALL(__pread_chk, fd, buf, n, off, bufsize);
}
ssize_t __pread64_chk(int fd, void *buf, size_t n, off64_t off,
                      size_t bufsize) {
  CALL(__pread64_chk, fd, buf, n, off, bufsize);
}
ssize_t write(int fd, const void *buf, size_t n) { CALL(write, fd, buf, n); }
ssize_t pwrite(int fd, const void *buf, size_t n, off_t off) {
  CALL(pwrite, fd, buf, n, off);
}
ssize_t pwrite64(int fd, const void *buf, size_t n, off64_t off) {
  CALL(pwrite64, fd, buf, n, off);
}
off_t lseek(int fd, off_t off, int whence) { CALL(lseek, fd, off, whence); }
off64_t lseek64(int fd, off64_t off, int whence) {
  CALL(lseek64, fd, off, whence);
}
int ftruncate(int fd, off_t len) { CALL(ftruncate, fd, len); }
int ftruncate64(int fd, off64_t len) { CALL(ftruncate64, fd, len); }
int fsync(int fd) { CALL(fsync, fd); }
int fdatasync(int fd) { CALL(fdatasync, fd); }
int fcntl(int fd, int cmd, ...) {
  void *ptr;
  va_list arg;
  va_start(arg, cmd);
  ptr = va_arg(arg, void *);
  va_end(arg);
  CALL(fcntl, fd, cmd, ptr);
}
int access(const char *path, int mode) { CALL(access, path, mode); }
int __fxstat(int ver, int fd, struct stat *buf) {
  CALL(__fxstat, ver, fd, buf);
}
int __fxstat64(int ver, int fd, struct stat64 *buf) {
  CALL(__fxstat64, ver, fd, buf);
}
int __xstat(int ver, const char *path, struct stat *buf) {
  CALL(__xstat, ver, path, buf);
}
int __xstat64(int ver, const char *path, struct stat64 *buf) {
  CALL(__xstat64, ver, path, buf);
}
int unlink(const char *path) { CALL(unlink, path); }
int mkdir(const char *path, mode_t mode) { CALL(mkdir, path, mode); }
int rmdir(const char *path) { CALL(rmdir, path); }
DIR *opendir(const char *path) { CALL(opendir, path); }
int closedir(DIR *dir) { CALL(closedir, dir); }
struct dirent *readdir(DIR *dir) {
  CALL(readdir, dir);
}
void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
  CALL(mmap, addr, len, prot, flags, fd, off);
}
void *mmap64(void *addr, size_t len, int prot, int flags, int fd, off64_t off) {
  CALL(mmap64, addr, len, prot, flags, fd, off);
}
int munmap(void *addr, size_t len) { CALL(munmap, addr, len); }
int mprotect(void *addr, size_t len, int prot) {
  CALL(mprotect, addr, len, prot);
}
int msync(void *addr, size_t len, int flags) { CALL(msync, addr, len, flags); }
int madvise(void *addr, size_t len, int advice) {
  CALL(madvise, addr, len, advice);
}

void __cyg_profile_func_enter(void *this_fn, void *call_site) {
  print_enter_trace(this_fn);
}

void __cyg_profile_func_exit(void *this_fn, void *call_site) {
  print_exit_trace(this_fn);
}
}
}  // namespace calltrace
