#ifndef LLVM_CLANG_SYNTAX_TYPEQUALIFIERS_H
#define LLVM_CLANG_SYNTAX_TYPEQUALIFIERS_H

#include "llvm/ADT/ArrayRef.h"
#include <memory>

namespace clang {
namespace syn {

class TypeQualifiers {
  unsigned qualifiers = 0;

public:
  enum : uint64_t {
    Const = 1 << 1,
    Immutable = 1 << 2,
    Volatile = 1 << 3,
  };

  enum : uint64_t {
    /// The maximum supported address space number.
    /// 23 bits should be enough for anyone.
    MaxAddressSpace = 0x7fffffu,

    /// The width of the "fast" qualifier mask.
    FastWidth = 3,

    /// The fast qualifier mask.
    FastMask = (1 << FastWidth) - 1
  };

public:
  bool HasConst() const { return qualifiers & TypeQualifiers::Const; }
  bool IsConst() const { return qualifiers == TypeQualifiers::Const; }
  void RemoveConst() { qualifiers &= ~TypeQualifiers::Const; }
  void AddConst() { qualifiers |= TypeQualifiers::Const; }

public:
  bool HasImmutable() const { return qualifiers & TypeQualifiers::Immutable; }
  bool IsImmutable() const { return qualifiers == TypeQualifiers::Immutable; }
  void RemoveImmutable() { qualifiers &= ~TypeQualifiers::Immutable; }
  void AddImmutable() { qualifiers |= TypeQualifiers::Immutable; }

public:
  bool HasVolatile() const { return qualifiers & TypeQualifiers::Volatile; }
  bool IsVolatile() const { return qualifiers == TypeQualifiers::Volatile; }
  void RemoveVolatile() { qualifiers &= ~TypeQualifiers::Volatile; }
  void AddVolatile() { qualifiers |= TypeQualifiers::Volatile; }
};

} // namespace syn
} // namespace clang

#endif