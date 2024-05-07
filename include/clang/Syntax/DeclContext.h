#ifndef LLVM_CLANG_SYNTAX_DECLCONTEXT_H
#define LLVM_CLANG_SYNTAX_DECLCONTEXT_H

#include "clang/Syntax/TypeAlignment.h"


#include "llvm/ADT/ArrayRef.h"
#include <memory>

namespace clang {
namespace syntax {

class alignas(1 << DeclAlignInBits) DeclContext {
public:

};
} // namespace syntax

} // end namespace clang

#endif
