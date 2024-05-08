#ifndef LLVM_CLANG_SYNTAX_DECLCONTEXT_H
#define LLVM_CLANG_SYNTAX_DECLCONTEXT_H

#include "clang/Core/SourceLocation.h"
#include "clang/Syntax/Spec.h"
#include "clang/Syntax/TypeAlignment.h"

#include "llvm/ADT/ArrayRef.h"
#include <memory>

namespace clang {
namespace syn {
class TypeSpec final {
  src::SourceLocation loc;
  TypeKind kind;

  void SetTypeSpec(TypeKind kind, src::SourceLocation inputLoc);

public:
  TypeSpec();

public:
  void AddVoid(src::SourceLocation inputLoc);
  bool IsVoid() const { return (loc.IsValid() && (kind == TypeKind::Void)); }
  void AddNull(src::SourceLocation inputLoc);
  bool IsNull() const { return (loc.IsValid() && (kind == TypeKind::Null)); }

  void AddBool(src::SourceLocation inputLoc);
  bool IsBool() const { return (loc.IsValid() && (kind == TypeKind::Bool)); }

  void AddChar(src::SourceLocation inputLoc);
  bool IsChar() const { return (loc.IsValid() && (kind == TypeKind::Char)); }
  void AddFloat(src::SourceLocation inputLoc);
  bool IsFloat() const { return (loc.IsValid() && (kind == TypeKind::Float)); }
  void AddFloat32(src::SourceLocation inputLoc);
  bool IsFloat32() const {
    return (loc.IsValid() && (kind == TypeKind::Float32));
  }
  void AddFloat64(src::SourceLocation inputLoc);
  bool IsFloat64() const {
    return (loc.IsValid() && (kind == TypeKind::Float64));
  }
  void AddInt(src::SourceLocation inputLoc);
  bool IsInt() const { return (loc.IsValid() && (kind == TypeKind::Int)); }
  void AddInt8(src::SourceLocation inputLoc);
  bool IsInt8() const { return (loc.IsValid() && (kind == TypeKind::Int8)); }
  void AddInt16(src::SourceLocation inputLoc);
  bool IsInt16() const { return (loc.IsValid() && (kind == TypeKind::Int16)); }
  void AddInt32(src::SourceLocation inputLoc);
  bool IsInt32() const { return (loc.IsValid() && (kind == TypeKind::Int32)); }
  void AddInt64(src::SourceLocation inputLoc);
  bool IsInt64() const { return (loc.IsValid() && (kind == TypeKind::Int64)); }

  // void AddString(src::SourceLocation inputLoc);
  // bool IsString() const {
  //   return (loc.IsValid() && (kind == TypeKind::String));
  // }
  void AddUInt(src::SourceLocation inputLoc);
  bool IsUInt() const { return (loc.IsValid() && (kind == TypeKind::UInt)); }
  void AddUInt8(src::SourceLocation inputLoc);
  bool IsUInt8() const { return (loc.IsValid() && (kind == TypeKind::UInt8)); }

  void AddUInt16(src::SourceLocation inputLoc);
  bool IsUInt16() const {
    return (loc.IsValid() && (kind == TypeKind::UInt16));
  }
  void AddUInt32(src::SourceLocation inputLoc);
  bool IsUInt32() const {
    return (loc.IsValid() && (kind == TypeKind::UInt32));
  }
  void AddUInt64(src::SourceLocation inputLoc);
  bool IsUInt64() const {
    return (loc.IsValid() && (kind == TypeKind::UInt64));
  }
  void AddComplex32(src::SourceLocation inputLoc);
  bool IsComplex32() const {
    return (loc.IsValid() && (kind == TypeKind::Complex32));
  }
  void AddComplex64(src::SourceLocation inputLoc);
  bool IsComplex64() const {
    return (loc.IsValid() && (kind == TypeKind::Complex64));
  }
  void AddImaginary32(src::SourceLocation inputLoc);
  bool IsImaginary32() const {
    return (loc.IsValid() && (kind == TypeKind::Imaginary32));
  }

  void AddImaginary64(src::SourceLocation inputLoc);
  bool IsImaginary64() const {
    return (loc.IsValid() && (kind == TypeKind::Imaginary64));
  }

  void AddEnum(src::SourceLocation inputLoc);
  bool IsEnum() { return (loc.IsValid() && kind == TypeKind::Enum); }

  void AddInterface(src::SourceLocation inputLoc);
  bool IsInterface() { return (loc.IsValid() && kind == TypeKind::Interface); }

  void AddStruct(src::SourceLocation inputLoc);
  bool IsStruct() { return (loc.IsValid() && kind == TypeKind::Struct); }

  src::SourceLocation GetLoc() { return loc; }
};

class DeclSpec {
  TypeSpec typeSpec;

public:
  DeclSpec();
};
} // namespace syn

} // end namespace clang

#endif
