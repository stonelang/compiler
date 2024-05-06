#ifndef LLVM_CLANG_FRONTEND_FRONTEND_H
#define LLVM_CLANG_FRONTEND_FRONTEND_H

#include "clang/Basic/LLVM.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Frontend/FrontendOptions.h"

#include "clang/APINotes/APINotesOptions.h"
#include "clang/Basic/CodeGenOptions.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/FileSystemOptions.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/LangStandard.h"
#include "clang/Frontend/DependencyOutputOptions.h"
#include "clang/Frontend/FrontendOptions.h"
#include "clang/Frontend/MigratorOptions.h"
#include "clang/Frontend/PreprocessorOutputOptions.h"
#include "clang/StaticAnalyzer/Core/AnalyzerOptions.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"

#include <memory>
#include <string>
#include <vector>

namespace llvm {
class Triple;
namespace opt {
class ArgList;
} // namespace opt
namespace vfs {
class FileSystem;
} // namespace vfs
} // namespace llvm

namespace clang {

class DiagnosticsEngine;
class TargetOptions;

class FrontendInvocation {

  /// Options controlling the language variant.
  std::unique_ptr<LangOptions> LangOpts;

  /// Options controlling the target.
  std::unique_ptr<TargetOptions> TargetOpts;

  /// Options controlling the diagnostic engine.
  IntrusiveRefCntPtr<DiagnosticOptions> DiagnosticOpts;

  /// Options controlling the static analyzer.
  AnalyzerOptionsRef AnalyzerOpts;

  std::unique_ptr<MigratorOptions> MigratorOpts;

  /// Options controlling API notes.
  std::unique_ptr<APINotesOptions> APINotesOpts;

  /// Options controlling IRgen and the backend.
  std::unique_ptr<CodeGenOptions> CodeGenOpts;

  /// Options controlling file system operations.
  std::unique_ptr<FileSystemOptions> FSOpts;

  /// Options controlling the frontend itself.
  std::unique_ptr<FrontendOptions> FrontendOpts;

  /// Options controlling dependency output.
  std::unique_ptr<DependencyOutputOptions> DependencyOutputOpts;

public:
public:
  // This lets us create the DiagnosticsEngine with a properly-filled-out
  // DiagnosticOptions instance.
  static std::unique_ptr<DiagnosticOptions>
  CreateAndPopulateDiagOpts(ArrayRef<const char *> argv);

  /// Fill out Opts based on the options given in Args.
  ///
  /// Args must have been created from the OptTable returned by
  /// createCC1OptTable().
  ///
  /// When errors are encountered, return false and, if Diags is non-null,
  /// report the error(s).
  static bool ParseDiagnosticArgs(DiagnosticOptions &diagOpts,
                                  llvm::opt::ArgList &args,
                                  DiagnosticsEngine *diags = nullptr,
                                  bool defaultDiagColor = true);
};

class FrontendInstance {
public:
};

} // namespace clang
#endif