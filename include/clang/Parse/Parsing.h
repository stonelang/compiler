#ifndef LLVM_CLANG_FRONTEND_PARSING_H
#define LLVM_CLANG_FRONTEND_PARSING_H

#include "clang/Parse/Parser.h"
#include "clang/Sema/DelayedDiagnostic.h"
#include "clang/Sema/ParsedTemplate.h"
#include "clang/Sema/Sema.h"

namespace clang {

class ParsingDeclSpec;

/// RAII object used to inform the actions that we're
/// currently parsing a declaration.  This is active when parsing a
/// variable's initializer, but not when parsing the body of a
/// class or function definition.
class ParsingDeclRAII {
  Sema &sema;
  sema::DelayedDiagnosticPool DiagnosticPool;
  Sema::ParsingDeclState State;
  bool Popped;

  ParsingDeclRAII(const ParsingDeclRAII &) = delete;
  void operator=(const ParsingDeclRAII &) = delete;

public:
  enum NoParent_t { NoParent };
  ParsingDeclRAII(Parser &P, NoParent_t _)
      : sema(P.GetSema()), DiagnosticPool(nullptr) {
    push();
  }

  /// Creates a RAII object whose pool is optionally parented by another.
  ParsingDeclRAII(Parser &P, const sema::DelayedDiagnosticPool *parentPool)
      : sema(P.GetSema()), DiagnosticPool(parentPool) {
    push();
  }

  /// Creates a RAII object and, optionally, initialize its
  /// diagnostics pool by stealing the diagnostics from another
  /// RAII object (which is assumed to be the current top pool).
  ParsingDeclRAII(Parser &P, ParsingDeclRAII *other)
      : sema(P.GetSema()),
        DiagnosticPool(other ? other->DiagnosticPool.getParent() : nullptr) {
    if (other) {
      DiagnosticPool.steal(other->DiagnosticPool);
      other->abort();
    }
    push();
  }

  ~ParsingDeclRAII() { abort(); }

  sema::DelayedDiagnosticPool &getDelayedDiagnosticPool() {
    return DiagnosticPool;
  }
  const sema::DelayedDiagnosticPool &getDelayedDiagnosticPool() const {
    return DiagnosticPool;
  }

  /// Resets the RAII object for a new declaration.
  void reset() {
    abort();
    push();
  }

  /// Signals that the context was completed without an appropriate
  /// declaration being parsed.
  void abort() { pop(nullptr); }

  void complete(Decl *D) {
    assert(!Popped && "ParsingDeclaration has already been popped!");
    pop(D);
  }

  /// Unregister this object from Sema, but remember all the
  /// diagnostics that were emitted into it.
  void abortAndRemember() { pop(nullptr); }

private:
  void push() {
    State = sema.PushParsingDeclaration(DiagnosticPool);
    Popped = false;
  }

  void pop(Decl *D) {
    if (!Popped) {
      sema.PopParsingDeclaration(State, D);
      Popped = true;
    }
  }
};

class ParsingDeclSpec final : public DeclSpec {
  ParsingDeclRAII parsingRAII;

public:
  const char *prevSpec = nullptr;
  unsigned diagID = 0;
  bool isTopLevelDecl = false;

public:
  ParsingDeclSpec(Parser &parser)
      : DeclSpec(parser.GetAttrFactory()),
        parsingRAII(parser, ParsingDeclRAII::NoParent) {}

  ParsingDeclSpec(Parser &parser, ParsingDeclRAII *RAII)
      : DeclSpec(parser.GetAttrFactory()), parsingRAII(parser, RAII) {}

public:
  const sema::DelayedDiagnosticPool &getDelayedDiagnosticPool() const {
    return parsingRAII.getDelayedDiagnosticPool();
  }

public:
  // High level wrappers

  void SetIntSpec(SourceLocation loc);
  void ClearIntSpec();
  bool HasIntSpec();

public:
  void Complete(Decl *D) { parsingRAII.complete(D); }
  void Abort() { parsingRAII.abort(); }
  bool HasAnySpec();
};

/// A class for parsing a declarator.
class ParsingDeclarator : public Declarator {
  ParsingDeclRAII parsingRAII;

public:
  ParsingDeclarator(Parser &parser, const ParsingDeclSpec &spec,
                    const ParsedAttributes &declarationAttrs,
                    DeclaratorContext C)
      : Declarator(spec, declarationAttrs, C),
        parsingRAII(parser, &spec.getDelayedDiagnosticPool()) {}

  const ParsingDeclSpec &getDeclSpec() const {
    return static_cast<const ParsingDeclSpec &>(Declarator::getDeclSpec());
  }

  ParsingDeclSpec &getMutableDeclSpec() const {
    return const_cast<ParsingDeclSpec &>(getDeclSpec());
  }

  void clear() {
    Declarator::clear();
    parsingRAII.reset();
  }

  void complete(Decl *D) { parsingRAII.complete(D); }
};

class ParsingScope final {
public:
  ParsingScope();
};

/// RAII object that makes '>' behave either as an operator
/// or as the closing angle bracket for a template argument list.
class GreaterThanIsOperatorScope {
  bool &GreaterThanIsOperator;
  bool OldGreaterThanIsOperator;

public:
  GreaterThanIsOperatorScope(bool &gtio, bool Val)
      : GreaterThanIsOperator(gtio), OldGreaterThanIsOperator(gtio) {
    GreaterThanIsOperator = Val;
  }

  ~GreaterThanIsOperatorScope() {
    GreaterThanIsOperator = OldGreaterThanIsOperator;
  }
};

/// RAII class that helps handle the parsing of an open/close delimiter
/// pair, such as braces { ... } or parentheses ( ... ).
class BalancedDelimiterTracker : public GreaterThanIsOperatorScope {
  Parser &P;
  tok::TokenKind Kind, Close, FinalToken;
  SourceLocation (Parser::*Consumer)();
  SourceLocation LOpen, LClose;

  unsigned short &getDepth() {
    switch (Kind) {
    case tok::l_brace:
      return P.BraceCount;
    case tok::l_square:
      return P.BracketCount;
    case tok::l_paren:
      return P.ParenCount;
    default:
      llvm_unreachable("Wrong token kind");
    }
  }

  bool diagnoseOverflow();
  bool diagnoseMissingClose();

public:
  BalancedDelimiterTracker(Parser &p, tok::TokenKind k,
                           tok::TokenKind FinalToken = tok::semi)
      : GreaterThanIsOperatorScope(p.GreaterThanIsOperator, true), P(p),
        Kind(k), FinalToken(FinalToken) {
    switch (Kind) {
    default:
      llvm_unreachable("Unexpected balanced token");
    case tok::l_brace:
      Close = tok::r_brace;
      Consumer = &Parser::ConsumeBrace;
      break;
    case tok::l_paren:
      Close = tok::r_paren;
      Consumer = &Parser::ConsumeParen;
      break;

    case tok::l_square:
      Close = tok::r_square;
      Consumer = &Parser::ConsumeBracket;
      break;
    }
  }

  SourceLocation getOpenLocation() const { return LOpen; }
  SourceLocation getCloseLocation() const { return LClose; }
  SourceRange getRange() const { return SourceRange(LOpen, LClose); }

  bool consumeOpen() {
    if (!P.Tok.is(Kind))
      return true;

    if (getDepth() < P.GetLangOpts().BracketDepth) {
      LOpen = (P.*Consumer)();
      return false;
    }

    return diagnoseOverflow();
  }

  bool expectAndConsume(unsigned DiagID = diag::err_expected,
                        const char *Msg = "",
                        tok::TokenKind SkipToTok = tok::unknown);
  bool consumeClose() {
    if (P.Tok.is(Close)) {
      LClose = (P.*Consumer)();
      return false;
    } else if (P.Tok.is(tok::semi) && P.PeekNextToken().is(Close)) {
      SourceLocation SemiLoc = P.ConsumeToken();
      P.Diag(SemiLoc, diag::err_unexpected_semi)
          << Close << FixItHint::CreateRemoval(SourceRange(SemiLoc, SemiLoc));
      LClose = (P.*Consumer)();
      return false;
    }

    return diagnoseMissingClose();
  }
  void skipToEnd();
};

} // namespace clang

#endif
