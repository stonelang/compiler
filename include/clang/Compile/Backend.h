

#ifndef LLVM_CLANG_CODEGEN_BACKEND_H
#define LLVM_CLANG_CODEGEN_BACKEND_H

#include "llvm/ADT/ArrayRef.h"
#include <memory>

namespace clang {
namespace codegen {

class Backend {
public:
  void EmitNative();
  void WriteNative();
};

}
