

#ifndef LLVM_CLANG_SYNTAX_CANTYPE_H
#define LLVM_CLANG_SYNTAX_CANTYPE_H

#include "clang/Syntax/Type.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/iterator.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/PointerLikeTypeTraits.h"
#include <cassert>
#include <iterator>
#include <type_traits>

namespace clang {
namespace syn {

template <typename T = Type> class CanType {
  /// The actual, canonical type.
  QualType underlyType;

public:
  // Constructs a NULL canonical type.
  CanType() = default;

  /// Converting constructor that permits implicit upcasting of
  /// canonical type pointers.
  template <typename U>
  CanType(const CanType<U> &other,
          std::enable_if_t<std::is_base_of<T, U>::value, int> = 0);

public:
  /// Retrieve the underlying type pointer, which refers to a
  /// canonical type.
  ///
  /// The underlying pointer must not be nullptr.
  // const T *GetTypePtr() const { return cast<T>(underlyType.getTypePtr()); }
};

} // namespace syn
} // namespace clang

#endif
