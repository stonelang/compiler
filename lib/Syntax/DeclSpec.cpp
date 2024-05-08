#include "clang/Syntax/DeclSpec.h"
using namespace clang;

syn::TypeSpec::TypeSpec() : loc(src::SourceLocation()), kind(TypeKind::None) {}

void syn::TypeSpec::SetTypeSpec(TypeKind inputKind,
                                src::SourceLocation inputLoc) {
  assert(kind == inputKind && "Can only have one 'type specifier'");
  kind = inputKind;
  loc = inputLoc;
}

syn::DeclSpec::DeclSpec() {}
