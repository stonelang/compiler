#ifndef LLVM_CLANG_CODEGEN_CODEGEN_H
#define LLVM_CLANG_CODEGEN_CODEGEN_H

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Target/TargetMachine.h"

#include "llvm/ADT/ArrayRef.h"

#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/ModuleDecl.h"

#include <memory>

namespace clang {
class SourceFile;
class ModuleDecl;
class CompilerInstance;

namespace codegen {

// class CodeGenAction;
// class CodeGenModule;
// class CodeGenExecution;
// class CodeGenPassManager;

// enum class CodeGenKind : unsigned {
//   EmitBC,       ///< Emit LLVM bitcode files
//   EmitIR,       ///< Generate IR and emit LLVM IR
//   EmitAssembly, ///< Generate IR and emit native assembly files
//   EmitObject    ///< Generate IR and then emit native object files
// };

// class CodeGenPassManager final {
//   llvm::PassBuilder pb;
//   llvm::LoopAnalysisManager lam;
//   llvm::FunctionAnalysisManager fam;
//   llvm::CGSCCAnalysisManager cgam;
//   llvm::ModuleAnalysisManager mam;
//   llvm::ModulePassManager mpm;
//   llvm::legacy::PassManager lpm;
//   llvm::legacy::FunctionPassManager lfpm;
//   llvm::FunctionPassManager fpm;
//   llvm::Module *llvmModule = nullptr;

// public:
//   CodeGenPassManager(const CodeGenPassManager &) = delete;
//   void operator=(const CodeGenPassManager &) = delete;
//   CodeGenPassManager(CodeGenPassManager &&) = delete;
//   void operator=(CodeGenPassManager &&) = delete;

// public:
//   explicit CodeGenPassManager(llvm::Module *llvmModule);

// public:
//   llvm::PassBuilder &GetPassBuilder() { return pb; }
//   llvm::LoopAnalysisManager &GetLoopAnalysisManager() { return lam; }
//   llvm::FunctionAnalysisManager &GetFunctionAnalysisManager() { return fam; }
//   llvm::CGSCCAnalysisManager &GetCGSCCAnalysisManager() { return cgam; }
//   llvm::ModuleAnalysisManager &GetModuleAnalysisManager() { return mam; }
//   llvm::ModulePassManager &GetPassManager() { return mpm; }
//   llvm::legacy::PassManager &GetLegacyPassManager() { return lpm; }
//   llvm::legacy::FunctionPassManager &GetLegacyFunctionPassManager() {
//     return lfpm;
//   }
//   llvm::FunctionPassManager &GetFunctionPassManager() { return fpm; }
//   llvm::Module *GetLLVMModule() { return llvmModule; }
// };


// struct CodeGenTypeCache final {

//   llvm::Type *VoidTy;
//   llvm::IntegerType *Int8Ty;  /// i8
//   llvm::IntegerType *Int16Ty; /// i16
//   llvm::IntegerType *Int32Ty; /// i32
//   llvm::IntegerType *Int64Ty; /// i64
//   llvm::IntegerType *IntTy;   /// int

//   llvm::PointerType *Int8PtrTy;  /// i16*
//   llvm::PointerType *Int16PtrTy; /// i16*
//   llvm::PointerType *Int32PtrTy; /// i32*
//   llvm::PointerType *Int64PtrTy; /// i64*
//   llvm::PointerType *IntPtrTy;   ///  int*

//   llvm::IntegerType *CharTy; /// char

//   // LLVM Address types
//   llvm::IntegerType *RelativeAddressTy;
//   llvm::PointerType *RelativeAddressPtrTy;

//   explicit CodeGenTypeCache(llvm::LLVMContext &llvmContext);
// };

// class CodeGenModule final {

//   CodeGenAction &codeGenAction;
//   CodeGenPassManager codeGenPassMgr;
//   CodeGenTypeCache codeGenTypeCache;

//   //std::unique_ptr<clang::CodeGenerator> clangCodeGen;

// public:
//   CodeGenModule(CodeGenAction &codeGenAction);

// public:
//   CodeGenAction &GetCodeGenAction() { return codeGenAction; }
//   CodeGenPassManager &GetCodeGenPassMgr() { return codeGenPassMgr; }
//   CodeGenTypeCache& GetCodeGenTypeCache() { return codeGenTypeCache;}
// };

// class CodeGenExecution final : public ASTConsumer {

//   CodeGenModule &codeGenModule;

// public:
//   CodeGenExecution(CodeGenModule &codeGenModule);

// private:
//   void GenerateIR(SourceFile *sourceFile);
//   void EmitIR(SourceFile *sourceFile);

//   void EmitBC();
//   void OptimizeIR();

// public:
//   void HandleCXXStaticMemberVarInstantiation(VarDecl *VD) override;
//   void Initialize(ASTContext &Ctx) override;
//   bool HandleSourceFile(SourceFile* S) override;
//   bool HandleModuleDecl(ModuleDecl* M) override;
//   bool HandleTopLevelDecl(DeclGroupRef D) override;
//   void HandleInlineFunctionDefinition(FunctionDecl *D) override;
//   void HandleInterestingDecl(DeclGroupRef D) override;
//   void HandleTranslationUnit(ASTContext &C) override;
//   void HandleTagDeclDefinition(TagDecl *D) override;
//   void HandleTagDeclRequiredDefinition(const TagDecl *D) override;
//   void CompleteTentativeDefinition(VarDecl *D) override;
//   void CompleteExternalDeclaration(VarDecl *D) override;
//   void AssignInheritanceModel(CXXRecordDecl *RD) override;
//   void HandleVTable(CXXRecordDecl *RD) override;
// };

// class CodeGenAction : public ASTFrontendAction {

//   CodeGenKind kind;
//   CodeGenModule codeGenModule;

// public:
//   CodeGenAction(CodeGenKind kind);

// public:
//   std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
//                                                  StringRef InFile) override;

//   CodeGenKind GetKind();

// public:
//   CodeGenModule &GetCodeGenModule() { return codeGenModule; }
// };

// class NativeEmitter {

// public:
//   void EmitOutputFile();
//   void WriteOutputFile();

// };

} // namespace codegen
} // namespace clang

#endif