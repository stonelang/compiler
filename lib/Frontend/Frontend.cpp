#include "clang/Frontend/Frontend.h"

using namespace clang;

FrontendInvocation::FrontendInvocation() {}

FrontendInstance::FrontendInstance(const FrontendInvocation &invocation)
    : invocation(invocation) {}