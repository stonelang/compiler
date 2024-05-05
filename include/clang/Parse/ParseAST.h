#ifndef LLVM_CLANG_PARSE_PARSESOURCEFILE_H
#define LLVM_CLANG_PARSE_PARSESOURCEFILE_H

#include "clang/Basic/LangOptions.h"

namespace clang {
class Sema;
class Preprocessor;
class ASTConsumer;
class ASTContext;
class ASTConsumer;
class SourceFile;
class CodeCompleteConsumer;

/// Parse the entire file specified, notifying the ASTConsumer as
/// the file is parsed.
///
/// This operation inserts the parsed decls into the translation
/// unit held by Ctx.
///
/// \param PrintStats Whether to print LLVM statistics related to parsing.
/// \param TUKind The kind of translation unit being parsed.
/// \param CompletionConsumer If given, an object to consume code completion
/// results.
/// \param SkipFunctionBodies Whether to skip parsing of function bodies.
/// This option can be used, for example, to speed up searches for
/// declarations/definitions when indexing.
void ParseAST(Preprocessor &pp, ASTConsumer *C, ASTContext &Ctx,
              bool PrintStats = false, TranslationUnitKind TUKind = TU_Complete,
              CodeCompleteConsumer *CompletionConsumer = nullptr,
              bool SkipFunctionBodies = false);

/// Parse the main file known to the preprocessor, producing an
/// abstract syntax tree.
void ParseAST(Sema &sem, bool printStats = false,
              bool skipFunctionBodies = false);

/// Parse the main file known to the preprocessor, producing an
/// abstract syntax tree.
void ParseAST(SourceFile &sourceFile, Sema &sem, bool printStats = false,
              bool skipFunctionBodies = false);

} // end namespace clang

#endif
