#ifndef LLVM_CLANG_PARSE_IDENTIFIERINFOCACHE_H
#define LLVM_CLANG_PARSE_IDENTIFIERINFOCACHE_H

#include <memory>

namespace clang {

class Parser;
class IdentifierInfo;

class IdentifierInfoCache final {
public:
  mutable IdentifierInfo *IdentifierFinal = nullptr;
  mutable IdentifierInfo *IdentifierInt = nullptr;
  mutable IdentifierInfo *IdentifierEnum = nullptr;

public:
  IdentifierInfoCache(Parser &parser);
};

} // end namespace clang

#endif
