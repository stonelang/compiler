#ifndef LLVM_CLANG_COMPILE_COMPILE_H
#define LLVM_CLANG_COMPILE_COMPILE_H

#include "llvm/ADT/ArrayRef.h"

namespace clang {

class CompilerConsumer;
int Compile(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
            CompilerConsumer *consumer = nullptr);

} // namespace clang

#endif