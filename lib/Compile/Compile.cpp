
#include "clang/Compile/Compile.h"
#include "clang/ARCMigrate/ARCMTActions.h"
#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Compile/CodeGen.h"
#include "clang/Config/config.h"
#include "clang/Driver/Options.h"
#include "clang/ExtractAPI/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Frontend/Utils.h"
#include "clang/Rewrite/Frontend/FrontendActions.h"
#include "clang/StaticAnalyzer/Frontend/AnalyzerHelpFlags.h"
#include "clang/StaticAnalyzer/Frontend/FrontendActions.h"

#include "llvm/Option/OptTable.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/BuryPointer.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/ErrorHandling.h"

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
std::unique_ptr<FrontendAction>
clang::CreateFrontendAction(CompilerInstance &clangInstance) {
  switch (clangInstance.getFrontendOpts().ProgramAction) {

  case frontend::ParseSyntaxOnly:
    return std::make_unique<SyntaxOnlyAction>();
  case frontend::EmitAssembly:
    return std::make_unique<codegen::CodeGenAction>(
        codegen::CodeGenKind::EmitAssembly);
  case frontend::EmitBC:
    return std::make_unique<EmitBCAction>();
  case frontend::EmitLLVM:
    return std::make_unique<EmitLLVMAction>();
  case frontend::EmitLLVMOnly:
    return std::make_unique<EmitLLVMOnlyAction>();
  case frontend::EmitCodeGenOnly:
    return std::make_unique<EmitCodeGenOnlyAction>();
  case frontend::EmitObj:
    return std::make_unique<EmitObjAction>();

  default:
    llvm_unreachable("Invalid compiler action!");
  }
}

bool clang::Compile(CompilerInstance &clangInstance) {

  if (clangInstance.getFrontendOpts().ShowHelp) {
    PrintCompilerHelp();
    return true;
  }

  clangInstance.LoadRequestedPlugins();
  if (clangInstance.getDiagnostics().hasErrorOccurred()) {
    return false;
  }
  auto frontendAction = clang::CreateFrontendAction(clangInstance);
  bool success = clangInstance.ExecuteAction(*frontendAction);

  if (clangInstance.getFrontendOpts().DisableFree) {
    llvm::BuryPointer(std::move(frontendAction));
  }
  return success;
}
