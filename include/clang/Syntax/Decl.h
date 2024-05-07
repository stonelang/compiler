#ifndef LLVM_CLANG_SYNTAX_DECL_H
#define LLVM_CLANG_SYNTAX_DECL_H

#include "clang/Core/InlineBitfield.h"
#include "clang/Syntax/ASTAllocation.h"

#include "llvm/ADT/ArrayRef.h"

#include <memory>

namespace clang {
namespace syntax {

enum class DeclKind : uint8_t {
#define DECL(DERIVED, BASE) DERIVED,
#define ABSTRACT_DECL(DECL)
#define DECL_RANGE(BASE, START, END) First##BASE = START, Last##BASE = END,
#define LAST_DECL_RANGE(BASE, START, END) First##BASE = START, Last##BASE = END
#include "clang/Syntax/DeclNode.inc"
};

 enum : unsigned { NumDeclKindBits =
   clang::CountBitsUsed(static_cast<unsigned>(DeclKind::LastDecl)) };

class Decl : public ASTAllocation<Decl> {
public:
};
} // namespace syntax

} // end namespace clang

#endif
