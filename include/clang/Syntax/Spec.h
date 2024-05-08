#ifndef LLVM_CLANG_SYNTAX_SPEC_H
#define LLVM_CLANG_SYNTAX_SPEC_H

#include "llvm/ADT/ArrayRef.h"
#include <memory>

namespace clang {
namespace syn {

enum class TypeKind : uint8_t {
#define TYPE(Class, Base) Class,
#define LAST_TYPE(Class) TypeLast = Class
#define ABSTRACT_TYPE(Class, Base)
#include "clang/Syntax/TypeNode.inc"

};

enum : unsigned {
  NumTypeKindBits =
      clang::CountBitsUsed(static_cast<unsigned>(TypeKind::TypeLast))
};

enum class AccessSpecKind : uint8_t {
  None = 0,
  Public,
  Internal,
  Private,
};

} // namespace syn
} // end namespace clang

#endif
