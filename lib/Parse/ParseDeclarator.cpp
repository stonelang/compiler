#include "clang/Parse/Parser.h"
#include "clang/Parse/Parsing.h"

using namespace clang;

void Parser::ParseDeclarator(Declarator &declarator) {

  DeclSpec localSpec(attrFactory);
  auto kind = GetTok().getKind();

  if (GetTok().IsStar()) {
    // Pointer '*' -> pointer
    auto loc = ConsumeToken();
    declarator.SetRangeEnd(loc);
    // Remember that we parsed a pointer type, and remember the type-quals.
    declarator.AddTypeInfo(
        DeclaratorChunk::getPointer(
            localSpec.getTypeQualifiers(), loc, localSpec.getConstSpecLoc(),
            localSpec.getVolatileSpecLoc(), localSpec.getRestrictSpecLoc(),
            localSpec.getAtomicSpecLoc(), localSpec.getUnalignedSpecLoc()),
        std::move(localSpec.getAttributes()), SourceLocation());
  } else if (GetTok().IsAmp()) {
    // A reference '&'
    auto loc = ConsumeToken();
    // Remember that we parsed a reference type.
    declarator.AddTypeInfo(
        DeclaratorChunk::getReference(localSpec.getTypeQualifiers(), loc,
                                      kind == tok::amp),
        std::move(localSpec.getAttributes()), SourceLocation());
  } else {
    // Direct -- calls the function declarator
    ParseDirectDeclarator(declarator);
  }
}

void Parser::ParseDirectDeclarator(Declarator &declarator) {

  assert(!GetTok().IsStar() && !GetTok().IsAmp());

  DirectDeclaratorScope directDeclaratorScope(*this,
                                              declarator.getCXXScopeSpec());

  if (GetTok().IsIdentifier()) {
  }

  // if (declarator.mayHaveIdentifier()) {
  // }
  // else if (Tok.is(tok::identifier) && declarator.mayHaveIdentifier()) {

  // } else if (Tok.is(tok::identifier) && !declarator.mayHaveIdentifier()) {

  // }
}

void Parser::ParseBracketDeclarator(Declarator &declarator) {}

void Parser::ParseParenDeclarator(Declarator &declarator) {
  BalancedDelimiterTracker delimiterTracker(*this, tok::l_paren);
  delimiterTracker.consumeOpen();
}

void Parser::ParseFunctionDeclarator(Declarator &D,
                                     ParsedAttributes &firstArgAttrs,
                                     BalancedDelimiterTracker &Tracker,
                                     bool isAmbiguous, bool requiresArg) {

  // Remember that we parsed a function type, and remember the attributes.
  // D.AddTypeInfo(DeclaratorChunk::getFunction(
  //                   HasProto, IsAmbiguous, LParenLoc, ParamInfo.data(),
  //                   ParamInfo.size(), EllipsisLoc, RParenLoc,
  //                   RefQualifierIsLValueRef, RefQualifierLoc,
  //                   /*MutableLoc=*/SourceLocation(),
  //                   ESpecType, ESpecRange, DynamicExceptions.data(),
  //                   DynamicExceptionRanges.data(), DynamicExceptions.size(),
  //                   NoexceptExpr.isUsable() ? NoexceptExpr.get() : nullptr,
  //                   ExceptionSpecTokens, DeclsInPrototype, StartLoc,
  //                   LocalEndLoc, D, TrailingReturnType,
  //                   TrailingReturnTypeLoc, &DS),
  //               std::move(FnAttrs), EndLoc);
}
