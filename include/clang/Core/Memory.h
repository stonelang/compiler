#ifndef LLVM_CLANG_CORE_MEMORY_H
#define LLVM_CLANG_CORE_MEMORY_H

#include <cassert>
#if defined(_WIN32)
#include <malloc.h>
#else
#include <cstdlib>
#endif

namespace clang {

inline void *AlignedAlloc(size_t size, size_t align) {
#if defined(_WIN32)
  void *r = _aligned_malloc(size, align);
  assert(r && "_aligned_malloc failed");
#elif __STDC_VERSION__ - 0 >= 201112l
  // C11 supports aligned_alloc
  void *r = aligned_alloc(align, size);
  assert(r && "aligned_alloc failed");
#else
  // posix_memalign only accepts alignments greater than sizeof(void*).
  if (align < sizeof(void *)) {
    align = sizeof(void *);
  }

  void *r = nullptr;
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

} // namespace clang
#endif
