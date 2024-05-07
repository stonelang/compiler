#include "clang/Syntax/ASTContext.h"
#include "clang/Basic/LangOptions.h"
using namespace clang;

void *syn::ASTContextAllocateMem(size_t bytes, const syn::ASTContext &ctx,
                                 AllocationArena arena, unsigned alignment) {}

syn::ASTContext::ASTContext(const LangOptions &langOpts)
    : langOpts(langOpts), identifiers(langOpts) {}
