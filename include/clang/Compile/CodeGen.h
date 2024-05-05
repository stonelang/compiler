

#ifndef LLVM_CLANG_CODEGEN_CODEGEN_H
#define LLVM_CLANG_CODEGEN_CODEGEN_H

#include "llvm/ADT/ArrayRef.h"
#include <memory>

namespace clang {
namespace codegen {

enum class CodeGenKind : unsigned {
  EmitBC,       ///< Emit LLVM bitcode files
  EmitIR,       ///< Generate IR and emit LLVM IR
  EmitAssembly, ///< Generate IR and emit native assembly files
  EmitObject    ///< Generate IR and then emit native object files
};

class CodeGenModule final {

  CodeGenAction &codeGenAction;

public:
  CodeGenModule(CodeGenAction &instance);

public:
  CodeGenAction &GetCodeGenAction() { return codeGenAction; }
};

class CodeGenASTConsumer : public ASTConsumer {

  CodeGenAction &codeGenAction;

public:
  CodeGenASTConsumer(CodeGenAction &instance);

public:
  void HandleSourceFile(SourceFile *sourceFile) override;
  void HandleModuleDecl(ModuleDecl *sourceFile) override;
};

class CodeGenAction : public ASTFrontendAction, public ASTConsumer {

  CodeGenKind kind;
  CodeGenModule codeGenModule;

public:
  CodeGenAction(CodeGenActionKind kind);
public:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef InFile) override
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

public:
  IRGenPassManager(const IRGenPassManager &) = delete;
  void operator=(const IRGenPassManager &) = delete;
  IRGenPassManager(IRGenPassManager &&) = delete;
  void operator=(IRGenPassManager &&) = delete;

public:
  IRGenPassManager();
  ~IRGenPassManager();

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
};

}

#endif