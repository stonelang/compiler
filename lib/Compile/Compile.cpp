#include "clang/Compile/Compile.h"
#include "clang/Compile/CompilerConsumer.h"
#include "clang/Compile/CompilerInstance.h"
#include "clang/Compile/CompilerInvocation.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"

#include <memory>

using namespace clang;

int clang::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, CompilerConsumer *consumer) {

  CompilerInvocation invocation;
  invocation.ParseCommandLine();

  CompilerInstance instance(invocation);
  instance.SetConsumer(consumer);

  return 0;
}
