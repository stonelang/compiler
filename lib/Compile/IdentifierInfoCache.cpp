#include "clang/Compile/IdentifierInfoCache.h"
#include "clang/Compile/Parser.h"

using namespace clang;

IdentifierInfoCache::IdentifierInfoCache(Parser &parser)
    : IdentifierFinal(parser.GetIdentifierInfo("final")) {}
