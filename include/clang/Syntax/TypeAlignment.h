#ifndef LLVM_CLANG_SYNTAX_TYPEALIGNMENT_H
#define LLVM_CLANG_SYNTAX_TYPEALIGNMENT_H

#include "llvm/ADT/ArrayRef.h"
#include <memory>

namespace clang {
namespace syn {
constexpr size_t DeclAlignInBits = 8;

// TODO: 4?
constexpr size_t TypeAlignInBits = 8;
} // namespace syn
} // namespace clang

#endif