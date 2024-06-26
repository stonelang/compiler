#ifndef LLVM_CLANG_CORE_DIAGNOSTS_H
#define LLVM_CLANG_CORE_DIAGNOSTS_H

#include "clang/Basic/Specifiers.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Compiler.h"

#include <cassert>
#include <cstdint>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace llvm {
class Error;
class raw_ostream;
} // namespace llvm

namespace clang {
namespace diags {
class DiagnosticEngine final {
public:
  // DiagnosticEngine(src::SourceManager& srcMgr)
};
} // namespace diags
} // namespace clang

#endif
