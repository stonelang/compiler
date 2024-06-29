#include "clang/Compile/Compile.h"
#include "clang/Compile/CompilerInstance.h"
#include "clang/Frontend/FrontendInstance.h"

#include <memory>
using namespace clang;

int clang::Compile() {

  FrontendInstance frontend;
  frontend.ParseCommandLine();

  CompilerInstance compiler(frontend);
  // compiler.ExecuteAction(frontend.GetMainAction());

  return 0;
}

// void CompilerInstance::ExecuteAction(FrontendAction requested) {

// }

void CompilerInstance::ExecutePrintHelpAction() {}

void CompilerInstance::ExecutePrintVersionAction() {}

void CompilerInstance::ExecuteParseAction() {}

void CompilerInstance::ExecutePrintASTAction() {}

void CompilerInstance::ExecuteTypeCheckAction() {}

void CompilerInstance::ExecuteEmitASTAction() {}

void CompilerInstance::ExecuteEmitBitCodeAction() {}

void CompilerInstance::ExecuteEmitObjectAction() {}

void CompilerInstance::ExecuteEmitAssemblyAction() {}
