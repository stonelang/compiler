#include "clang/Compile/CompilerInstance.h"

using namespace clang;

CompilerInstance::CompilerInstance(const FrontendInstance &frontend)
    : frontend(frontend) {}