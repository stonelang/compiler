#ifndef LLVM_CLANG_COMPILE_COMPILEREXECUTION_H
#define LLVM_CLANG_COMPILE_COMPILEREXECUTION_H

#include "clang/Compile/CompilerConsumer.h"
#include "clang/Compile/CompilerInvocation.h"

#include <functional>
#include <optional>
namespace clang {
class SourceFile;

class CompilerContext final {
  // mutable ModuleDecl mainModuleDecl = nullptr;

public:
};

class CompilerInstance final {
  const CompilerInvocation &invocation;
  std::unique_ptr<CompilerContext> compilerContext;

  CompilerConsumer *compilerConsumer = nullptr;

public:
  CompilerInstance(const CompilerInvocation &invocation);

public:
  const CompilerInvocation &GetFrontend() { return invocation; }
  void SetConsumer(CompilerConsumer *consumer) { compilerConsumer = consumer; }
  CompilerConsumer *GetConsumer() { return compilerConsumer; }

  // public:
  //   void PerformPrintHelpAction();
  //   void PerformPrintVersionAction();
  //   void PerformParseAction();
  //   void PerformPrintASTAction();
  //   void PerformTypeCheckAction();
  //   void PerformEmitASTAction();
  //   void PerformEmitBitCodeAction();
  //   void PerformEmitObjectAction();
  //   void PerformEmitAssemblyAction();
};

class SourceFile {};

class CompilerActionConsumer {



};
class CompilerAction {

public:
};

// class SourceFileCompletionCallback {
// public:
//   CompletedSourceFile(SourceFile &sourceFile);

// };
// class ModuleDeclCompletionCallback {
// public:
//   CompletedModuleDecl(SourceFile &sourceFile);

// };

// class LLVMModuleCompletionCallback {
// public:
//   CompletedModuleDecl(llvm::Module* mo);

// };

// class ParseCompilerAction final : public CompilerAction {
// public:
//   ParseCompilerAction();
//   ParseCompilerAction(std::function<void(SourceFile &)> notify);
// };

// class TypeCheckCompilerAction final : public CompilerAction, public
// SourceFileCompletionCallback {

// public:
//   void CompletedSourceFile(SourceFile &sourceFile);

//   ParseCompilerAction GetDependency() {
//     return ParseCompilerAction(
//         [&](SourceFile &sourceFile) { HandleSourceFile(sourceFile); });
//   }
// };


class SourceFileConsumer {
public:
  virtual CompletedSourceFile(SourceFile &sourceFile) = 0;
};

class ParseCompilerAction final : public CompilerAction {
  SourceFileConsumer *callback = nullptr;

public:
  ParseCompilerAction(SourceFileConsumer *consumer = nullptr);
};

class TypeCheckCompilerAction final : public CompilerAction,
                                      public SourceFileConsumer {
public:
  void CompletedSourceFile(SourceFile &sourceFile) override; 


  ParseCompilerAction GetDependency() { return ParseCompilerAction(this); }
};


class PrintASTCompilerAction final : public CompilerAction {

   ParseCompilerAction GetDependency() { return ParseCompilerAction(this); }

};






} // namespace clang

#endif