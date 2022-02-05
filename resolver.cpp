// adopted from https://github.com/TomaszAugustyn/call-stack-logger

#include "resolver.h"

#if __has_include(<bfd.h>)

#include <bfd.h>
#include <dlfcn.h>

#include <map>
#include <memory>

namespace traceio {

#define DEFINE_FN(handle, fn) \
  static auto fn = reinterpret_cast<decltype(::fn)*>(dlsym(handle, #fn))

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

void resolve(void* address, Dl_info& info, const char*& fn_name,
             const char*& filename, int& line) {
  if (bfd_so == nullptr) return;
  BfdWrapper& bfd_wrapper = get_bfd(info);
  asection* section = bfd_wrapper.bfd->sections;
  const bool relative =
      section->vma < reinterpret_cast<uintptr_t>(info.dli_fbase);
  while (section != nullptr) {
    intptr_t offset = reinterpret_cast<intptr_t>(address) -
                      static_cast<intptr_t>(section->vma);
    if (relative) offset -= reinterpret_cast<intptr_t>(info.dli_fbase);
    if (offset < 0 || static_cast<size_t>(offset) > section->size) {
      section = section->next;
      continue;
    }
    bfd_find_nearest_line(bfd_wrapper.bfd.get(), section,
                          bfd_wrapper.symbols.get(), offset, &filename,
                          &fn_name, reinterpret_cast<unsigned int*>(&line));
    return;
  }
}

}  // namespace traceio
#else
#warning "BFD not found. Please install BFD via `sudo apt install binutils-dev`"

namespace traceio {
void resolve([[maybe_unused]] void* address, [[maybe_unused]] Dl_info& info,
             [[maybe_unused]] const char*& fn_name,
             [[maybe_unused]] const char*& filename,
             [[maybe_unused]] int& line) {}
}  // namespace traceio
#endif
