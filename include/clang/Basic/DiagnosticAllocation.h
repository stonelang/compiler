#ifndef CLANG_BASIC_DIAGNOSTICALLOCATION_H
#define CLANG_BASIC_DIAGNOSTICALLOCATION_H

#include "llvm/Support/Allocator.h"

#include <cassert>
#include <cstring>
#include <memory>

namespace clang {

class DiagnosticEngine;
void *AllocateInDiagnosticEngine(size_t bytes,
                                 const DiagnosticEngine &diagEngine,
                                 unsigned alignment);

/// Types inheriting from this class are intended to be allocated in an
/// \c ASTContext allocator; you cannot allocate them by using a normal \c
/// new, and instead you must either provide an \c ASTContext or use a placement
/// \c new.
///
/// The template parameter is a type with the desired alignment. It is usually,
/// but not always, the type that is inheriting \c ASTAllocated.
template <typename AlignTy> class DiagnosticAllocation {
public:
  // Make vanilla new/delete illegal.
  void *operator new(size_t bytes) throw() = delete;
  void operator delete(void *data) throw() = delete;

  // Only allow allocation using the allocator in MemoryContext
  // or by doing a placement new.
  void *operator new(size_t bytes, const DiagnosticEngine &diagEngine,
                     unsigned alignment = alignof(AlignTy)) {
    return clang::AllocateInDiagnosticEngine(bytes, diagEngine, alignment);
  }
  void *operator new(size_t bytes, void *mem) throw() {
    assert(mem && "placement new into failed allocation");
    return mem;
  }
};

} // namespace clang

#endif