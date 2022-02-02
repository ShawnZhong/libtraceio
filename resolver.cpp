// adopted from https://github.com/TomaszAugustyn/call-stack-logger

#include "resolver.h"

#include <bfd.h>  // sudo apt install binutils-dev
#include <dlfcn.h>
#include <elfutils/libdwfl.h>
#include <unistd.h>

#include <cassert>
#include <map>
#include <memory>

namespace traceio {

#define DEFINE_FN(handle, fn) \
  static auto fn = reinterpret_cast<decltype(::fn)*>(dlsym(handle, #fn))

static auto dw_so = dlopen("libdw.so", RTLD_LAZY | RTLD_DEEPBIND);
DEFINE_FN(dw_so, dwfl_begin);
DEFINE_FN(dw_so, dwfl_linux_proc_report);
DEFINE_FN(dw_so, dwfl_module_getsrc);
DEFINE_FN(dw_so, dwfl_report_end);
DEFINE_FN(dw_so, dwfl_lineinfo);
DEFINE_FN(dw_so, dwfl_addrmodule);
DEFINE_FN(dw_so, dwfl_standard_find_debuginfo);
DEFINE_FN(dw_so, dwfl_module_addrname);
DEFINE_FN(dw_so, dwfl_linux_proc_find_elf);

static auto bfd_so = dlopen("libbfd.so", RTLD_LAZY | RTLD_DEEPBIND);
DEFINE_FN(bfd_so, bfd_openr);
DEFINE_FN(bfd_so, bfd_close);
DEFINE_FN(bfd_so, bfd_check_format);

struct BfdWrapper {
  std::unique_ptr<struct bfd, decltype(bfd_close)> bfd;
  std::unique_ptr<asymbol*[]> symbols;

  explicit BfdWrapper(const char* filename)
      : bfd(bfd_openr(filename, nullptr), bfd_close) {
    if (bfd == nullptr) throw std::runtime_error("bfd_openr failed");
    bfd_check_format(bfd.get(), bfd_object);
    long size = bfd_get_symtab_upper_bound(bfd.get());
    if (size < 0) throw std::runtime_error("bfd_get_symtab_upper_bound failed");
    symbols.reset(reinterpret_cast<asymbol**>(new char[size]));
    bfd_canonicalize_symtab(bfd.get(), symbols.get());
  }
};

static std::map<void*, BfdWrapper> bfds;

static BfdWrapper& get_bfd(Dl_info& info) {
  if (auto it = bfds.find(info.dli_fbase); it != bfds.end()) return it->second;
  auto [it, success] = bfds.emplace(info.dli_fbase, BfdWrapper(info.dli_fname));
  return it->second;
}

static Dwfl* get_dwfl() {
  Dwfl* dwfl;
  Dwfl_Callbacks callbacks = {};
  char* debuginfo_path = nullptr;
  callbacks.find_elf = dwfl_linux_proc_find_elf;
  callbacks.find_debuginfo = dwfl_standard_find_debuginfo;
  callbacks.debuginfo_path = &debuginfo_path;
  dwfl = dwfl_begin(&callbacks);
  assert(dwfl);
  int r;
  r = dwfl_linux_proc_report(dwfl, getpid());
  assert(!r);
  r = dwfl_report_end(dwfl, nullptr, nullptr);
  assert(!r);
  static_cast<void>(r);
  return dwfl;
}

Dwfl* dwfl = nullptr;

extern "C" void resolve(void* address, Dl_info& info, const char*& fn_name,
                        const char*& filename, int& line) {
  get_dwfl();
  auto ip2 = reinterpret_cast<uintptr_t>(address);
  Dwfl_Module* module = dwfl_addrmodule(dwfl, ip2);
  fn_name = dwfl_module_addrname(module, ip2);
  if (Dwfl_Line* dwfl_line = dwfl_module_getsrc(module, ip2)) {
    filename =
        dwfl_lineinfo(dwfl_line, nullptr, &line, nullptr, nullptr, nullptr);
  }
}

}  // namespace traceio
