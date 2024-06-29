#ifndef LLVM_CLANG_COMPILE_COMPILEREXECUTION_H
#define LLVM_CLANG_COMPILE_COMPILEREXECUTION_H

#include "clang/Frontend/FrontendInstance.h"

#include <optional>
namespace clang {

class CompilerContext final {
  // mutable ModuleDecl mainModuleDecl = nullptr;

public:
};

class CompilerInstance final {
  const FrontendInstance &frontend;
  std::unique_ptr<CompilerContext> compilerContext;

public:
  CompilerInstance(const FrontendInstance &frontend);

public:
  const FrontendInstance &GetFrontend() { return frontend; }
  // CompilerContext &GetCompilerContext() { return *compilerContext; }

  bool ExecuteAction(FrontendAction action);

private:
  void ExecutePrintHelpAction();
  void ExecutePrintVersionAction();
  void ExecuteParseAction();
  void ExecutePrintASTAction();
  void ExecuteTypeCheckAction();
  void ExecuteEmitASTAction();
  void ExecutePrintIRAction();
  void ExecuteEmitIRAction();
  void ExecuteEmitBitCodeAction();
  void ExecuteEmitObjectAction();
  void ExecuteEmitAssemblyAction();
};

} // namespace clang

#endif