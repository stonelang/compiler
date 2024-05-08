#ifndef LLVM_CLANG_PARSE_LEXER_H
#define LLVM_CLANG_PARSE_LEXER_H

#include "clang/Basic/DiagnosticLex.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/LangOptions.h"

#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"

namespace clang {

class Lexer final {
  const FileID bufferID;
  SourceManager &sm;

public:
  Lexer(const FileID bufferID, SourceManager &sm);
};

} // end namespace clang

#endif
