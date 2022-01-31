#pragma once

#include <dlfcn.h>

namespace calltrace {
void resolve(void* address, Dl_info& info, const char*& fn_name,
             const char*& filename, int& line);
}  // namespace calltrace
