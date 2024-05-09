#ifndef LLVM_CLANG_COMPILE_COMPILEREXECUTION_H
#define LLVM_CLANG_COMPILE_COMPILEREXECUTION_H


#include "clang/Frontend/CompilerInvocation.h"

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

class DiagnosticEngine;
class TargetOptions;

class CompilerExecution final {
  const CompilerInvocation &invocation;

public:
  CompilerExecution(const CompilerInvocation &invocation);
};

} // namespace clang
#endif