#include "clang/Compile/Compile.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/CompilerExecution.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendDiagnostic.h"

using namespace clang;
using namespace llvm::opt;

// using namespace clang::codegen;

static void PrintCompilerHelp() {
  driver::getDriverOptTable().printHelp(
      llvm::outs(), "clang -compile [options] file...",
      "LLVM 'Clang' Compiler: http://clang.llvm.org",
      /*Include=*/driver::options::CC1Option,
      /*Exclude=*/0, /*ShowAllAliases=*/false);
}

bool clang::Compile(CompilerInstance &clangInstance) {

  if (clangInstance.getFrontendOpts().ShowHelp) {
    PrintCompilerHelp();
    return true;
  }

  CompilerExecution execution(clangInstance);
  if (!execution.Initialize()) {
    return false;
  }

  // Always happends
  execution.ExecuteCodeAnalysis();
}

// bool clang::ExecuteAction() {}

// bool clang::ExecuteCodeAnalysis() { return true; }

// bool clang::SetupCodeGeneration() {}

// bool clang::ExecuteCodeGeneration() {}

// bool clang::ExecuteIRGeneration() {}

// bool clang::ExecuteIROptimization() {}

bool clang::ExecuteNativeGeneration() {}

bool clang::ExecuteCompileLLVM() {}
