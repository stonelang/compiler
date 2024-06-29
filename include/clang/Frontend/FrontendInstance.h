#ifndef LLVM_CLANG_FRONTEND_FRONTENDINSTANCE_H
#define LLVM_CLANG_FRONTEND_FRONTENDINSTANCE_H

#include "clang/Frontend/FrontendOptions.h"

namespace clang {

class FrontendInstance final {
  FrontendOptions frontendOpts;

public:
  bool ParseCommandLine();

public:
  const FrontendOptions &GetFrontendOptions() { return frontendOpts; }
  FrontendAction GetMainAction() { return frontendOpts.mainAction; }
};

} // namespace clang

#endif