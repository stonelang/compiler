#ifndef CLANG_BASIC_MEMORY_H
#define CLANG_BASIC_MEMORY_H

#include <cassert>
#include <cstring>
#include <memory>

#if defined(_WIN32)
#include <malloc.h>
#else
#include <cstdlib>
#endif

namespace clang {

// FIXME: Use C11 aligned_alloc if available.
inline void *AlignedAlloc(size_t size, size_t align) {
  // posix_memalign only accepts alignments greater than sizeof(void*).
  //
  if (align < sizeof(void *)) {
    align = sizeof(void *);
  }

  void *r;
#if defined(_WIN32)
  r = _aligned_malloc(size, align);
  assert(r && "_aligned_malloc failed");
#else
  int res = posix_memalign(&r, align, size);
  assert(res == 0 && "posix_memalign failed");
  (void)res; // Silence the unused variable warning.
#endif
  return r;
}

inline void AlignedFree(void *p) {
#if defined(_WIN32)
  _aligned_free(p);
#else
  free(p);
#endif
}

inline void *Copy(void *dest, const void *src, std::size_t count) {
  return std::memcpy(dest, src, count);
}

} // namespace clang

#endif