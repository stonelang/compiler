#ifndef LLVM_CLANG_SYNTAX_ASTCONTEXT_H
#define LLVM_CLANG_SYNTAX_ASTCONTEXT_H

#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Syntax/ASTAllocation.h"

#include "llvm/ADT/ArrayRef.h"
#include <memory>

namespace clang {
namespace syn {

class ASTContext final {
  const LangOptions &langOpts;
  IdentifierTable identifiers;

public:
  ASTContext(const LangOptions &langOpts);

public:
  const LangOptions &GetLangOpts() const { return langOpts; }

  IdentifierInfo *GetIdentifier(StringRef Name) {
    return &identifiers.get(Name);
  }
};
} // namespace syn

} // end namespace clang

#endif
