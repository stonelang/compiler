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

public:
  Type(TypeKind kind) : kind(kind) {}
};

class BuilitinType : public Type {

public:
  BuilitinType(TypeKind kind) : Type(kind) {}
};

class NumberType : public BuilitinType {

public:
  NumberType(TypeKind kind) : BuilitinType(kind) {}
};

class IntType : public NumberType {

public:
  IntType() : NumberType(TypeKind::Int) {}
};

class Int8Type : public NumberType {

public:
  Int8Type() : NumberType(TypeKind::Int8) {}
};
class Int16Type : public NumberType {

public:
  Int16Type() : NumberType(TypeKind::Int16) {}
};
class Int32Type : public NumberType {

public:
  Int32Type() : NumberType(TypeKind::Int32) {}
};

class Int64Type : public NumberType {

public:
  Int64Type() : NumberType(TypeKind::Int64) {}
};

class Int128Type : public NumberType {

public:
  Int128Type() : NumberType(TypeKind::Int128) {}
};

class UIntType : public NumberType {

public:
  UIntType() : NumberType(TypeKind::UInt) {}
};

class UInt8Type : public NumberType {

public:
  UInt8Type() : NumberType(TypeKind::UInt8) {}
};
class UInt16Type : public NumberType {

public:
  UInt16Type() : NumberType(TypeKind::UInt16) {}
};
class UInt32Type : public NumberType {

public:
  UInt32Type() : NumberType(TypeKind::UInt32) {}
};

class UInt64Type : public NumberType {

public:
  UInt64Type() : NumberType(TypeKind::UInt64) {}
};

class UInt128Type : public NumberType {

public:
  UInt128Type() : NumberType(TypeKind::UInt128) {}
};

class FloatType : public NumberType {

public:
  FloatType() : NumberType(TypeKind::Float) {}
};
class Float16Type : public NumberType {

public:
  Float16Type() : NumberType(TypeKind::Float16) {}
};
class Float32Type : public NumberType {

public:
  Float32Type() : NumberType(TypeKind::Float32) {}
};
class Float64Type : public NumberType {

public:
  Float64Type() : NumberType(TypeKind::Float64) {}
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

class EnumType : public Type {
public:
  EnumType() : Type(TypeKind::Enum) {}
};

class StructType : public Type {
public:
  StructType() : Type(TypeKind::Struct) {}
};

class InterfaceType : public Type {
public:
  InterfaceType() : Type(TypeKind::Interface) {}
};

} // namespace syn

} // end namespace clang

#endif
