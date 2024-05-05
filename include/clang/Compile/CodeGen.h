#ifndef LLVM_CLANG_CODEGEN_CODEGEN_H
#define LLVM_CLANG_CODEGEN_CODEGEN_H

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Target/TargetMachine.h"

#include "llvm/ADT/ArrayRef.h"

#include "clang/Frontend/FrontendAction.h"

#include "clang/Frontend/CompilerInstance.h"

#include <memory>

namespace clang {

class SourceFile;
class ModuleDecl;

class CompilerInstance;

namespace codegen {

class CodeGenAction;
class CodeGenModule;

enum class CodeGenKind : unsigned {
  EmitBC,       ///< Emit LLVM bitcode files
  EmitIR,       ///< Generate IR and emit LLVM IR
  EmitAssembly, ///< Generate IR and emit native assembly files
  EmitObject    ///< Generate IR and then emit native object files
};

class CodeGenModule final {

  CodeGenAction &codeGenAction;

  CodeGenPassManager codeGenPassMgr; 

  // std::unique_ptr<clang::CodeGenerator> clangCodeGen;

public:
  CodeGenModule(CodeGenAction &codeGenAction);

public:
  CodeGenAction &GetCodeGenAction() { return codeGenAction; }
  CodeGenPassManager& GetCodeGenPassMgr() { return codeGenPassMgr;}
};

class CodeGenExecution final : public ASTConsumer {

  CodeGenModule &codeGenModule;

public:
  CodeGenExecution(CodeGenModule &codeGenModule);

public:
  void HandleSourceFile(SourceFile *sourceFile) override;
  void HandleModuleDecl(ModuleDecl *sourceFile) override;

private:
  void GenerateIR(SourceFile *sourceFile);
  void EmitIR(SourceFile *sourceFile);

  void EmitBC();
  void OptimizeIR();
  void EmitNativeOutput();
  void WriteNativeOutput();
};

class CodeGenAction : public ASTFrontendAction {

  CodeGenKind kind;
  CodeGenModule codeGenModule;

public:
  CodeGenAction(CodeGenKind kind);

public:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef InFile) override;

  CodeGenKind GetKind();

public:
  CodeGenModule &GetCodeGenModule() { return codeGenModule; }
};

class CodeGenPassManager final {
  llvm::PassBuilder pb;
  llvm::LoopAnalysisManager lam;
  llvm::FunctionAnalysisManager fam;
  llvm::CGSCCAnalysisManager cgam;
  llvm::ModuleAnalysisManager mam;
  llvm::ModulePassManager mpm;
  llvm::legacy::PassManager lpm;
  llvm::legacy::FunctionPassManager lfpm;
  llvm::FunctionPassManager fpm;
  llvm::Module *llvmModule = nullptr;
public:
  CodeGenPassManager(const CodeGenPassManager &) = delete;
  void operator=(const CodeGenPassManager &) = delete;
  CodeGenPassManager(CodeGenPassManager &&) = delete;
  void operator=(CodeGenPassManager &&) = delete;

public:
  explicit CodeGenPassManager(llvm::Module *llvmModule);
  ~CodeGenPassManager() = delete;

public:
  llvm::PassBuilder &GetPassBuilder() { return pb; }
  llvm::LoopAnalysisManager &GetLoopAnalysisManager() { return lam; }
  llvm::FunctionAnalysisManager &GetFunctionAnalysisManager() { return fam; }
  llvm::CGSCCAnalysisManager &GetCGSCCAnalysisManager() { return cgam; }
  llvm::ModuleAnalysisManager &GetModuleAnalysisManager() { return mam; }
  llvm::ModulePassManager &GetPassManager() { return mpm; }
  llvm::legacy::PassManager &GetLegacyPassManager() { return lpm; }
  llvm::legacy::FunctionPassManager &GetLegacyFunctionPassManager() {
    return lfpm;
  }
  llvm::FunctionPassManager &GetFunctionPassManager() { return fpm; }
  llvm::Module *GetLLVMModule() { return llvmModule; }
};

} // namespace codegen
} // namespace clang

#endif