#ifndef LLVM_CLANG_SYNTAX_ASTCONTEXT_H
#define LLVM_CLANG_SYNTAX_ASTCONTEXT_H

#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Syntax/ASTAllocation.h"
#include "clang/Syntax/CanType.h"
#include "clang/Syntax/Type.h"

#include "llvm/ADT/ArrayRef.h"
#include <memory>

namespace clang {
namespace syn {

class ASTContext final {
  const LangOptions &langOpts;
  IdentifierTable identifiers;

private:
  mutable llvm::SmallVector<Type *, 0> Types;
  // mutable llvm::FoldingSet<ComplexType> ComplexTypes;
  // mutable llvm::FoldingSet<PointerType>
  // PointerTypes{GeneralTypesLog2InitSize}; mutable
  // llvm::FoldingSet<BlockPointerType> BlockPointerTypes; mutable
  // llvm::FoldingSet<LValueReferenceType> LValueReferenceTypes; mutable
  // llvm::FoldingSet<RValueReferenceType> RValueReferenceTypes; mutable
  // llvm::FoldingSet<MemberPointerType> MemberPointerTypes;

private:
  const CanType<Type> BuiltinFloat16Type;  /// 32-bit IEEE floating point
  const CanType<Type> BuiltinFloat32Type;  /// 32-bit IEEE floating point
  const CanType<Type> BuiltinFloat64Type;  /// 64-bit IEEE floating point
  const CanType<Type> BuiltinFloat128Type; /// 128-bit IEEE floating point
  const CanType<Type> BuiltinFloatType;    /// 128-bit IEEE floating point

  const CanType<Type> BuiltinInt8Type;
  const CanType<Type> BuiltinInt16Type;
  const CanType<Type> BuiltinInt32Type;
  const CanType<Type> BuiltinInt64Type;
  const CanType<Type> BuiltinInt128Type;
  const CanType<Type> BuiltinIntType;

  const CanType<Type> BuiltinUInt8Type;
  const CanType<Type> BuiltinUInt16Type;
  const CanType<Type> BuiltinUInt32Type;
  const CanType<Type> BuiltinUInt64Type;
  const CanType<Type> BuiltinUInt128Type;
  const CanType<Type> BuiltinUIntType;

  const CanType<Type> BuiltinAnyType;
  const CanType<Type> BuiltinVoidType;
  const CanType<Type> BuiltinNullType;
  const CanType<Type> BuiltinBoolType;

  void AddBuiltinType(const CanType<Type> &canType, TypeKind kind);

public:
  ASTContext(const LangOptions &langOpts);

public:
  const LangOptions &GetLangOpts() const { return langOpts; }

  IdentifierInfo *GetIdentifier(StringRef Name) {
    return &identifiers.get(Name);
  }

public:
};
} // namespace syn

} // end namespace clang

#endif
