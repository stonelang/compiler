#include "clang/Compile/Lexer.h"

using namespace clang;

Lexer::Lexer(const FileID bufferID, SourceManager &sm)
    : bufferID(bufferID), sm(sm) {}