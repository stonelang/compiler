#ifndef LLVM_CLANG_COMPILER_COMPILERINSTANCE_H
#define LLVM_CLANG_COMPILER_COMPILERINSTANCE_H

#include "clang/Compile/CompilerOptions.h"

namespace clang {

class CompilerInvocation final {
  CompilerOptions compilerOpts;

public:
  bool ParseCommandLine();

public:
  const CompilerOptions &GetCompilerOptions() { return compilerOpts; }
  CompilerActionKind GetMainActionKind() { return compilerOpts.mainActionKind; }
};

} // namespace clang

#endif