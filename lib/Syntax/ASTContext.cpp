#include "clang/Syntax/ASTContext.h"
#include "clang/Basic/LangOptions.h"
using namespace clang;

void *syn::ASTContextAllocateMem(size_t bytes, const syn::ASTContext &ctx,
                                 AllocationArena arena, unsigned alignment) {}

syn::ASTContext::ASTContext(const LangOptions &langOpts)
    : langOpts(langOpts), identifiers(langOpts) {

  AddBuiltinType(BuiltinFloat16Type, TypeKind::Float16);
  AddBuiltinType(BuiltinFloat32Type, TypeKind::Float32);
  AddBuiltinType(BuiltinFloat64Type, TypeKind::Float64);
  AddBuiltinType(BuiltinFloatType, TypeKind::Float);

  AddBuiltinType(BuiltinInt8Type, TypeKind::Int8);
  AddBuiltinType(BuiltinInt16Type, TypeKind::Int16);
  AddBuiltinType(BuiltinInt32Type, TypeKind::Int32);
  AddBuiltinType(BuiltinInt64Type, TypeKind::Int64);
  AddBuiltinType(BuiltinInt128Type, TypeKind::Int128);
  AddBuiltinType(BuiltinIntType, TypeKind::Int);
}

void syn::ASTContext::AddBuiltinType(const CanType<Type> &canType,
                                     TypeKind kind) {}