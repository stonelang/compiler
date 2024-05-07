#ifndef LLVM_CLANG_SYNTAX_ASTALLOCATION_H
#define LLVM_CLANG_SYNTAX_ASTALLOCATION_H

#include "llvm/Support/Allocator.h"

#include <cassert>
#include <cstring>
#include <memory>

#if defined(_WIN32)
#include <malloc.h>
#else
#include <cstdlib>
#endif

namespace clang {
namespace syn {

class ASTContext;

/// The arena in which a particular allocation object will go.
enum class AllocationArena {
  /// The permanent arena, which is tied to the lifetime of
  /// the object
  ///
  /// All global declarations and types need to be allocated into this arena.
  /// At present, everything that is not a type involving a type variable is
  /// allocated in this arena.
  Permanent = 0,
  /// The constraint solver's temporary arena, which is tied to the
  /// lifetime of a particular instance of the constraint solver.
  ///
  /// Any type involving a type variable is allocated in this arena.
  Temporary
};

void *ASTContextAllocateMem(size_t bytes, const syn::ASTContext &ctx,
                            AllocationArena arena, unsigned alignment);

template <typename AlignTy> class ASTAllocation {

public:
  // Make vanilla new/delete illegal.
  void *operator new(size_t bytes) throw() = delete;
  void operator delete(void *data) throw() = delete;

  // Only allow allocation using the allocator in MemoryContext
  // or by doing a placement new.
  void *operator new(size_t bytes, const ASTContext &ctx,
                     AllocationArena arena = AllocationArena::Permanent,
                     unsigned alignment = alignof(AlignTy)) {

    return clang::syn::ASTContextAllocateMem(bytes, ctx, arena, alignment);
  }
  void *operator new(size_t bytes, void *mem) throw() {
    assert(mem && "placement new into failed allocation");
    return mem;
  }
};
} // namespace syn
} // end namespace clang

#endif
