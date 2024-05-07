#ifndef LLVM_CLANG_COMPILE_SEM_H
#define LLVM_CLANG_COMPILE_SEM_H

#include "llvm/ADT/ArrayRef.h"
#include <memory>

namespace clang {
namespace sem {
class Decl;
class Scope;
class NamedDecl;
class DeclContext;

class Sem final {

public:
  Decl *InitiateDeclarator(Scope *scope, Declarator &D);
  NamedDecl *InitiateFunctionDeclarator(Scope *S, Declarator &D,
                                        DeclContext *dc);

  Decl *InitiateFunctionDefinition();

public:
  void CheckDecl();

}; // namespace sem

} // end namespace clang

#endif