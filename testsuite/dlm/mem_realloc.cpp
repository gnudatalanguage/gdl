// A loadable module exercising the GDL memory API across a normal DLM call.
#include "typedefs.hpp"
#include "gdl_export.h"

#include <algorithm>
#include <cstddef>

extern "C" {
void* IDL_MemAlloc(EXPORT_MEMINT, const char*, int);
void* IDL_MemRealloc(void*, EXPORT_MEMINT, const char*, int);
void IDL_MemFree(void*, const char*, int);
EXPORT_VPTR IDL_GettmpLong(EXPORT_LONG);
}

extern "C" EXPORT_VPTR check_mem_realloc(int, EXPORT_VPTR*, char*) {
  constexpr std::size_t count = 97;
  for (int i = 0; i < 64; ++i) {
    auto* ptr = static_cast<unsigned char*>(IDL_MemAlloc(count, nullptr, 0));
    if (!ptr) return IDL_GettmpLong(0);
    for (std::size_t j = 0; j < count; ++j) ptr[j] = static_cast<unsigned char>(j + i);

    for (const auto size : {count * 8, count / 2}) {
      ptr = static_cast<unsigned char*>(IDL_MemRealloc(ptr, size, nullptr, 0));
      if (!ptr) return IDL_GettmpLong(0);
      for (std::size_t j = 0; j < std::min(size, count); ++j)
        if (ptr[j] != static_cast<unsigned char>(j + i)) return IDL_GettmpLong(0);
    }
    // The DLM cleanup owns the other half of the resized allocations.
    if (i % 2 == 0) IDL_MemFree(ptr, nullptr, 0);
  }
  void* ptr = IDL_MemRealloc(nullptr, count, nullptr, 0);
  return IDL_GettmpLong(ptr && IDL_MemRealloc(ptr, 0, nullptr, 0) == nullptr);
}
