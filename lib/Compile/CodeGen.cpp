#include "clang/Compile/CodeGen.h"

using namespace clang;


codegen::CodeGenModule::CodeGenModule(codegen::CodeGenAction &codeGenAction)
    : codeGenAction(codeGenAction), codeGenPassMgr(nullptr) {}

codegen::CodeGenAction::CodeGenAction(codegen::CodeGenKind kind)
    : kind(kind), codeGenModule(*this) {}

std::unique_ptr<ASTConsumer>
codegen::CodeGenAction::CreateASTConsumer(CompilerInstance &CI, StringRef InFile) {

  return std::make_unique<codegen::CodeGenExecution>(GetCodeGenModule());
}

codegen::CodeGenPassManager::CodeGenPassManager(llvm::Module *llvmModule)
    : lfpm(llvmModule) {
  // Register all the ctx analyses with the managers.
  pb.registerModuleAnalyses(mam);
  pb.registerCGSCCAnalyses(cgam);
  pb.registerFunctionAnalyses(fam);
  pb.registerLoopAnalyses(lam);
  pb.crossRegisterProxies(lam, fam, cgam, mam);
  // mpm = pb.buildPerModuleDefaultPipeline(codeGenOpts.GetOptimizationLevel());
}

// void CodeGenExecution::GenerateIR() {
	
	//}

// void CodeGenExecution::EmitIR() {}

void codegen::CodeGenExecution::EmitBC() {}

void codegen::CodeGenExecution::OptimizeIR() {}
