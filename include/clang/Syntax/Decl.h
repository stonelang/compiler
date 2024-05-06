#ifndef LLVM_CLANG_SYNTAX_DECL_H
#define LLVM_CLANG_SYNTAX_DECL_H

#include "clang/Syntax/ASTAllocation.h"

#include "llvm/ADT/ArrayRef.h"

#include <memory>

namespace clang {
namespace syntax {

class Decl : public ASTAllocation<Decl> {
public:
};
} // namespace syntax

} // end namespace clang

#endif
