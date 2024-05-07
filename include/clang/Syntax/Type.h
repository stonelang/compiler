#ifndef LLVM_CLANG_SYNTAX_TYPE_H
#define LLVM_CLANG_SYNTAX_TYPE_H

#include "clang/Core/InlineBitfield.h"
#include "clang/Syntax/ASTAllocation.h"
#include "clang/Syntax/TypeAlignment.h"

#include "llvm/ADT/ArrayRef.h"

#include <memory>

namespace clang {
namespace syn {
class Type;
// Provide forward declarations for all of the *Type classes.
#define TYPE(Class, Base) class Class##Type;
#include "clang/Syntax/TypeNode.inc"

enum : uint64_t {
  /// The maximum supported address space number.
  /// 23 bits should be enough for anyone.
  MaxAddressSpace = 0x7fffffu
};

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

class QualType {
  // Thankfully, these are efficiently composable.
  // llvm::PointerIntPair<const Type *, TypeQuals::FastWidth> val;
public:
  QualType() = default;
  // QualType(const Type *typePtr, unsigned typeQuals) : val(typePtr, typeQuals)
  // {}
};

class alignas(1 << TypeAlignInBits) Type
    : public syn::ASTAllocation<std::aligned_storage<8, 8>::type> {
  TypeKind kind;

protected:
  union {

    CLANG_INLINE_BITFIELD_BASE(Type, clang::BitMax(NumTypeKindBits, 8) + 1, Kind
                               : clang::BitMax(NumTypeKindBits, 8),

                                 /// Whether this declaration is invalid.
                                 IsBuiltin : 1);
  } Bits;

public:
  Type(TypeKind kind) : kind(kind) {}
};

class BuilitinType : public Type {

public:
  BuilitinType(TypeKind kind) : Type(kind) {}
};

class VoidType final : public BuilitinType {
public:
  VoidType() : BuilitinType(TypeKind::Void) {}
};

class NullType final : public BuilitinType {
public:
  NullType() : BuilitinType(TypeKind::Null) {}
};

class NumericType : public BuilitinType {

public:
  NumericType(TypeKind kind) : BuilitinType(kind) {}
};

class SignedType : public NumericType {

public:
  SignedType(TypeKind kind) : NumericType(kind) {}
};

class IntType final : public SignedType {

public:
  IntType() : SignedType(TypeKind::Int) {}
};

class Int8Type final : public SignedType {

public:
  Int8Type() : SignedType(TypeKind::Int8) {}
};
class Int16Type final : public SignedType {

public:
  Int16Type() : SignedType(TypeKind::Int16) {}
};
class Int32Type final : public SignedType {

public:
  Int32Type() : SignedType(TypeKind::Int32) {}
};

class Int64Type final : public SignedType {

public:
  Int64Type() : SignedType(TypeKind::Int64) {}
};

class Int128Type final : public SignedType {

public:
  Int128Type() : SignedType(TypeKind::Int128) {}
};

class UnsignedType : public NumericType {

public:
  UnsignedType(TypeKind kind) : NumericType(kind) {}
};

class UIntType final : public UnsignedType {

public:
  UIntType() : UnsignedType(TypeKind::UInt) {}
};

class UInt8Type final : public UnsignedType {

public:
  UInt8Type() : UnsignedType(TypeKind::UInt8) {}
};
class UInt16Type final : public UnsignedType {

public:
  UInt16Type() : UnsignedType(TypeKind::UInt16) {}
};
class UInt32Type final : public UnsignedType {

public:
  UInt32Type() : UnsignedType(TypeKind::UInt32) {}
};

class UInt64Type final : public UnsignedType {

public:
  UInt64Type() : UnsignedType(TypeKind::UInt64) {}
};

class UInt128Type final : public UnsignedType {

public:
  UInt128Type() : UnsignedType(TypeKind::UInt128) {}
};

class FloatType final : public NumericType {

public:
  FloatType() : NumericType(TypeKind::Float) {}
};
class Float16Type final : public NumericType {

public:
  Float16Type() : NumericType(TypeKind::Float16) {}
};
class Float32Type final : public NumericType {

public:
  Float32Type() : NumericType(TypeKind::Float32) {}
};
class Float64Type final : public NumericType {

public:
  Float64Type() : NumericType(TypeKind::Float64) {}
};

class FunctionType : public Type {

public:
  FunctionType(TypeKind kind) : Type(kind) {}
};

class FunType : public Type {

public:
  FunType() : Type(TypeKind::Fun) {}
};

class PointerType : public Type {
public:
  PointerType() : Type(TypeKind::Pointer) {}
};

class BlockPointerType : public Type {
public:
  BlockPointerType() : Type(TypeKind::BlockPointer) {}
};

class MemberPointerType : public Type {
public:
  MemberPointerType() : Type(TypeKind::MemberPointer) {}
};

class ReferenceType : public Type {
public:
  ReferenceType(TypeKind kind) : Type(kind) {}
};

class LValueReferenceType : public Type {
public:
  LValueReferenceType() : Type(TypeKind::LValueReference) {}
};

class RValueReferenceType : public Type {
public:
  RValueReferenceType() : Type(TypeKind::RValueReference) {}
};

class NominalType : public Type {
public:
  NominalType(TypeKind kind) : Type(kind) {}
};

class EnumType : public NominalType {
public:
  EnumType() : NominalType(TypeKind::Enum) {}
};

class StructType : public NominalType {
public:
  StructType() : NominalType(TypeKind::Struct) {}
};

class InterfaceType : public NominalType {
public:
  InterfaceType() : NominalType(TypeKind::Interface) {}
};

class SugaredType : public Type {
public:
  SugaredType(TypeKind kind) : Type(kind) {}
};

class AliasType : public SugaredType {
public:
  AliasType() : SugaredType(TypeKind::Alias) {}
};

} // namespace syn

} // end namespace clang

#endif
