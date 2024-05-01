#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ExternalASTSource.h"
#include "clang/AST/Stmt.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Parse/Parser.h"
#include "clang/Sema/CodeCompleteConsumer.h"
#include "clang/Sema/EnterExpressionEvaluationContext.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/SemaConsumer.h"
#include "clang/Sema/TemplateInstCallback.h"

#include "llvm/Support/CrashRecoveryContext.h"
#include "llvm/Support/TimeProfiler.h"

#include <cstdio>
#include <memory>

using namespace clang;

void clang::ParseAST(Preprocessor &lexer, ASTConsumer *astConsumer,
                              ASTContext &astContext, bool PrintStats,
                              TranslationUnitKind TUKind,
                              CodeCompleteConsumer *CompletionConsumer,
                              bool SkipFunctionBodies) {

  std::unique_ptr<Sema> sema(
      new Sema(lexer, astContext, *astConsumer, TUKind, CompletionConsumer));
  // Recover resources if we crash before exiting this method.
  llvm::CrashRecoveryContextCleanupRegistrar<Sema> CleanupSema(sema.get());

  clang::ParseAST(*sema.get(), PrintStats, SkipFunctionBodies);
}

void clang::ParseAST(Sema &sema, bool printStats,
                              bool skipFunctionBodies) {

  // Collect global stats on Decls/Stmts (until we have a module streamer).
  if (printStats) {
    Decl::EnableStatistics();
    Stmt::EnableStatistics();
  }

  // Also turn on collection of stats inside of the Sema object.
  bool OldCollectStats = printStats;
  std::swap(OldCollectStats, sema.CollectStats);

  sema.getPreprocessor().EnterMainSourceFile();
  assert(sema.getPreprocessor().getCurrentLexer());

  Parser parser(sema, skipFunctionBodies);

  llvm::SmallVector<ParserResult<Decl>> topLevelDecls;
  parser.ParseTopLevelDecls(topLevelDecls);

  auto astConsumer = &sema.getASTConsumer();
  for (auto topLevelDecl : topLevelDecls) {
    if (topLevelDecl.IsNull()) {
      return;
    }
    astConsumer->HandleTopLevelDecl(
        sema.ConvertDeclToDeclGroup(topLevelDecl.Get()).get());
  }
  astConsumer->HandleTranslationUnit(sema.getASTContext());

  std::swap(OldCollectStats, sema.CollectStats);
  if (printStats) {
    llvm::errs() << "\nSTATISTICS:\n";
    parser.GetSema().PrintStats();
    parser.GetSema().getASTContext().PrintStats();

    Decl::PrintStats();
    Stmt::PrintStats();
  }
}

/// Parse the main file known to the preprocessor, producing an
/// abstract syntax tree.
void clang::ParseAST(SourceFile &sourceFile, Sema &sem,
                              bool printStats, bool skipFunctionBodies) {}
