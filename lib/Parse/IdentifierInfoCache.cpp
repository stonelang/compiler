#include "clang/Parse/IdentifierInfoCache.h"
#include "clang/Parse/Parser.h"

using namespace clang;

IdentifierInfoCache::IdentifierInfoCache(Parser &parser)
    : IdentifierFinal(parser.GetIdentifierInfo("final")) {}
