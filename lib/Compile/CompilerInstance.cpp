#include "clang/Compile/CompilerInstance.h"

using namespace clang;

CompilerInstance::CompilerInstance(const CompilerInvocation &invocation)
    : invocation(invocation) {}