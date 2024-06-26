#ifndef LLVM_CLANG_SYNTAX_DECLCONTEXT_H
#define LLVM_CLANG_SYNTAX_DECLCONTEXT_H

#include "clang/Syntax/TypeAlignment.h"

#include "llvm/ADT/ArrayRef.h"
#include <memory>

namespace clang {
namespace syn {

class alignas(1 << DeclAlignInBits) DeclContext {
public:
};
} // namespace syn

} // end namespace clang

#endif
