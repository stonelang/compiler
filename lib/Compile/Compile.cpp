
#include "clang/ARCMigrate/ARCMTActions.h"
#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Config/config.h"
#include "clang/Driver/Options.h"
#include "clang/ExtractAPI/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Frontend/Utils.h"
#include "clang/Compile/Compile.h"
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


static void PrintCompilerHelp() {
  driver::getDriverOptTable().printHelp(
        llvm::outs(), "clang -compile [options] file...",
        "LLVM 'Clang' Compiler: http://clang.llvm.org",
        /*Include=*/driver::options::CC1Option,
        /*Exclude=*/0, /*ShowAllAliases=*/false);
}
std::unique_ptr<FrontendAction> clang::CreateFrontendAction(CompilerInstance &clangInstance) {
switch (clangInstance.getFrontendOpts().ProgramAction) {
  case frontend::ASTDeclList:            return std::make_unique<ASTDeclListAction>();
  case frontend::ASTDump:                return std::make_unique<ASTDumpAction>();
  case frontend::ASTPrint:               return std::make_unique<ASTPrintAction>();
  case frontend::ASTView:                return std::make_unique<ASTViewAction>();
  case frontend::DumpCompilerOptions:
    return std::make_unique<DumpCompilerOptionsAction>();
  case frontend::EmitAssembly:           return std::make_unique<EmitAssemblyAction>();
  case frontend::EmitBC:                 return std::make_unique<EmitBCAction>();
  case frontend::EmitLLVM:               return std::make_unique<EmitLLVMAction>();
  case frontend::EmitLLVMOnly:           return std::make_unique<EmitLLVMOnlyAction>();
  case frontend::EmitCodeGenOnly:        return std::make_unique<EmitCodeGenOnlyAction>();
  case frontend::EmitObj:                return std::make_unique<EmitObjAction>();
  case frontend::FixIt:                  return std::make_unique<FixItAction>();
  case frontend::GenerateModule:
    return std::make_unique<GenerateModuleFromModuleMapAction>();
  case frontend::GenerateModuleInterface:
    return std::make_unique<GenerateModuleInterfaceAction>();
  case frontend::GenerateHeaderUnit:
    return std::make_unique<GenerateHeaderUnitAction>();
  case frontend::GeneratePCH:            return std::make_unique<GeneratePCHAction>();
  case frontend::InitOnly:               return std::make_unique<InitOnlyAction>();
  case frontend::ParseSyntaxOnly:        return std::make_unique<SyntaxOnlyAction>();
  case frontend::ModuleFileInfo:         return std::make_unique<DumpModuleInfoAction>();
  case frontend::VerifyPCH:              return std::make_unique<VerifyPCHAction>();
  default:
    llvm_unreachable("Invalid compiler action!");
  }
}

bool clang::Compile(CompilerInstance& clangInstance) {

  if(clangInstance.getFrontendOpts().ShowHelp){
    PrintCompilerHelp();
    return true;
  }

  clangInstance.LoadRequestedPlugins();
  if (clangInstance.getDiagnostics().hasErrorOccurred()){
    return false;
  }
  auto frontendAction = clang::CreateFrontendAction(clangInstance);
  bool success = clangInstance.ExecuteAction(*frontendAction);

  if (clangInstance.getFrontendOpts().DisableFree){
    llvm::BuryPointer(std::move(frontendAction));
  }
  return success; 
}

