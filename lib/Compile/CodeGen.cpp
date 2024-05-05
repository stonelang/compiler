#include "clang/Compile/CodeGen.h"


// codegen::CodeGenModule::CodeGenModule(codegen::CodeGenAction &codeGenAction)
//     : codeGenAction(codeGenAction), codeGenPassMgr(nullptr), codeGenTypeCache() {}

// codegen::CodeGenAction::CodeGenAction(codegen::CodeGenKind kind)
//     : kind(kind), codeGenModule(*this) {}

// std::unique_ptr<ASTConsumer>
// codegen::CodeGenAction::CreateASTConsumer(CompilerInstance &CI,
//                                           StringRef InFile) {

//   return std::make_unique<codegen::CodeGenExecution>(GetCodeGenModule());
// }

// codegen::CodeGenPassManager::CodeGenPassManager(llvm::Module *llvmModule)
//     : lfpm(llvmModule) {
//   // Register all the ctx analyses with the managers.
//   pb.registerModuleAnalyses(mam);
//   pb.registerCGSCCAnalyses(cgam);
//   pb.registerFunctionAnalyses(fam);
//   pb.registerLoopAnalyses(lam);
//   pb.crossRegisterProxies(lam, fam, cgam, mam);
//   // mpm = pb.buildPerModuleDefaultPipeline(codeGenOpts.GetOptimizationLevel());
// }

// // TODO: Ok for now -- may move to IRGenMoulde
// CodeGenTypeCache::CodeGenTypeCache(llvm::LLVMContext &llvmContext) {

//   VoidTy = llvm::Type::getVoidTy(llvmContext);

//   // Int types
//   Int8Ty = llvm::Type::getInt8Ty(llvmContext);
//   Int16Ty = llvm::Type::getInt16Ty(llvmContext);
//   Int32Ty = llvm::Type::getInt32Ty(llvmContext);
//   Int64Ty = llvm::Type::getInt64Ty(llvmContext);

//   // IntPtr types
//   Int8PtrTy = Int8Ty->getPointerTo();
//   Int16PtrTy = Int16Ty->getPointerTo();
//   Int32PtrTy = Int32Ty->getPointerTo();
//   Int64PtrTy = Int16Ty->getPointerTo();
// }

// void CodeGenExecution::GenerateIR() {

//}

// void CodeGenExecution::EmitIR() {}

// codegen::CodeGenExecution::CodeGenExecution(codegen::CodeGenModule &codeGenModule)
//     : codeGenModule(codeGenModule) {}

// void codegen::CodeGenExecution::EmitBC() {}

// void codegen::CodeGenExecution::OptimizeIR() {}
