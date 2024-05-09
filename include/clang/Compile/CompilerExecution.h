#ifndef LLVM_CLANG_COMPILE_COMPILEREXECUTION_H
#define LLVM_CLANG_COMPILE_COMPILEREXECUTION_H

#include "clang/Frontend/CompilerInstance.h"

namespace clang {

class CompilerExecution final {
  CompilerInstance &instance;

public:
  CompilerExecution(CompilerInstance &instance);
  ~CompilerExecution();

public:
  bool Initialize();

public:
  bool ExecuteCodeAnalysis();

};


} // namespace clang
#endif