#include "clang/Parse/Parser.h"
#include "clang/Parse/Parsing.h"

using namespace clang;

Parser::Parser(Sema &sema, bool skipFunctionBodies)
    : lexer(sema.getPreprocessor()), sema(sema),
      PreferredType(lexer.isCodeCompletionEnabled()),
      diags(lexer.getDiagnostics()), GreaterThanIsOperator(true),
      ColonIsSacred(false), TemplateParameterDepth(0),
      identifierInfoCache(*this) {

  skipFunctionBodies = lexer.isCodeCompletionEnabled() || skipFunctionBodies;

  Tok.startToken();
  Tok.setKind(tok::eof);
  lexer.setCodeCompletionHandler(*this);

  sema.CurScope = nullptr;
  assert(GetCurScope() == nullptr && "A scope is already active?");
  EnterScope(Scope::DeclScope);
  sema.ActOnTranslationUnitScope(GetCurScope());
  sema.Initialize();

  // Prime the lexer look-ahead.
  ConsumeToken();
}
Parser::~Parser() {}

//===----------------------------------------------------------------------===//
// Scope manipulation
//===----------------------------------------------------------------------===//

/// EnterScope - Start a new scope.
void Parser::EnterScope(unsigned scopeFlags) {

  // Create the new scope
  auto scope = CreateScope(scopeFlags);

  // Make sure we have a scope
  assert(scope);

  // Cache the scope
  PushCurScope(scope);
}

void Parser::PushCurScope(Scope *scope) {
  sema.CurScope = scope;
  scopeCache.push_back(scope);
}

/// ExitScope - Pop a scope off the scope stack.
void Parser::ExitScope() {
  assert(GetCurScope() && "Scope imbalance!");
  // Inform the actions module that this scope is going away if there are any
  // decls in it.
  sema.ActOnPopScope(Tok.getLocation(), GetCurScope());

  Scope *oldScope = GetCurScope();
  sema.CurScope = oldScope->getParent();

  PopCurScope();
}

Scope *Parser::CreateScope(unsigned scopeFlags) {
  return new (sema.getASTContext()) Scope(GetCurScope(), scopeFlags, diags);
}

/// Set the flags for the current scope to ScopeFlags. If ManageFlags is false,
/// this object does nothing.
Parser::ParsingScopeFlags::ParsingScopeFlags(Parser *Self, unsigned ScopeFlags,
                                             bool ManageFlags)
    : CurScope(ManageFlags ? Self->GetCurScope() : nullptr) {
  if (CurScope) {
    OldFlags = CurScope->getFlags();
    CurScope->setFlags(ScopeFlags);
  }
}

/// Restore the flags for the current scope to what they were before this
/// object overrode them.
Parser::ParsingScopeFlags::~ParsingScopeFlags() {
  if (CurScope) {
    CurScope->setFlags(OldFlags);
  }
}

DiagnosticBuilder Parser::Diag(SourceLocation Loc, unsigned DiagID) {
  return diags.Report(Loc, DiagID);
}

DiagnosticBuilder Parser::Diag(const Token &Tok, unsigned DiagID) {
  return Diag(Tok.getLocation(), DiagID);
}

SourceLocation Parser::HandleUnexpectedCodeCompletionToken() {
  assert(GetTok().IsCodeCompletion());
  PrevTokLocation = Tok.getLocation();

  for (Scope *S = GetCurScope(); S; S = S->getParent()) {
    if (S->isFunctionScope()) {
      CutOffParsing();
      sema.CodeCompleteOrdinaryName(GetCurScope(),
                                    Sema::PCC_RecoveryInFunction);
      return PrevTokLocation;
    }
    if (S->isClassScope()) {
      CutOffParsing();
      sema.CodeCompleteOrdinaryName(GetCurScope(), Sema::PCC_Class);
      return PrevTokLocation;
    }
  }
  CutOffParsing();
  sema.CodeCompleteOrdinaryName(GetCurScope(), Sema::PCC_Namespace);
  return PrevTokLocation;
}

bool BalancedDelimiterTracker::diagnoseOverflow() {
  P.Diag(P.Tok, diag::err_bracket_depth_exceeded)
      << P.GetLangOpts().BracketDepth;
  P.Diag(P.Tok, diag::note_bracket_depth);
  P.CutOffParsing();
  return true;
}

bool BalancedDelimiterTracker::expectAndConsume(unsigned DiagID,
                                                const char *Msg,
                                                tok::TokenKind SkipToTok) {
  LOpen = P.Tok.getLocation();
  if (P.ExpectAndConsume(Kind, DiagID, Msg)) {
    if (SkipToTok != tok::unknown) {
      P.SkipUntil(SkipToTok, Parser::StopAtSemi);
    }
    return true;
  }
  if (getDepth() < P.GetLangOpts().BracketDepth) {
    return false;
  }

  return diagnoseOverflow();
}

bool BalancedDelimiterTracker::diagnoseMissingClose() {
  assert(!P.Tok.is(Close) && "Should have consumed closing delimiter");

  if (P.Tok.is(tok::annot_module_end))
    P.Diag(P.Tok, diag::err_missing_before_module_end) << Close;
  else
    P.Diag(P.Tok, diag::err_expected) << Close;
  P.Diag(LOpen, diag::note_matching) << Kind;

  // If we're not already at some kind of closing bracket, skip to our closing
  // token.
  if (P.Tok.isNot(tok::r_paren) && P.Tok.isNot(tok::r_brace) &&
      P.Tok.isNot(tok::r_square) &&
      P.SkipUntil(Close, FinalToken,
                  Parser::StopAtSemi | Parser::StopBeforeMatch) &&
      P.Tok.is(Close))
    LClose = P.ConsumeAnyToken();
  return true;
}

void BalancedDelimiterTracker::skipToEnd() {
  P.SkipUntil(Close, Parser::StopBeforeMatch);
  consumeClose();
}

static bool IsCommonTypo(tok::TokenKind ExpectedTok, const Token &Tok) {
  switch (ExpectedTok) {
  case tok::semi:
    return Tok.is(tok::colon) || Tok.is(tok::comma); // : or , for ;
  default:
    return false;
  }
}

bool Parser::ExpectAndConsume(tok::TokenKind ExpectedTok, unsigned DiagID,
                              StringRef Msg) {

  if (Tok.is(ExpectedTok) || Tok.is(tok::code_completion)) {
    ConsumeAnyToken();
    return false;
  }

  // Detect common single-character typos and resume.
  if (IsCommonTypo(ExpectedTok, Tok)) {
    SourceLocation Loc = Tok.getLocation();
    {
      DiagnosticBuilder DB = Diag(Loc, DiagID);
      DB << FixItHint::CreateReplacement(
          SourceRange(Loc), tok::getPunctuatorSpelling(ExpectedTok));
      if (DiagID == diag::err_expected)
        DB << ExpectedTok;
      else if (DiagID == diag::err_expected_after)
        DB << Msg << ExpectedTok;
      else
        DB << Msg;
    }

    // Pretend there wasn't a problem.
    ConsumeAnyToken();
    return false;
  }

  SourceLocation EndLoc = lexer.getLocForEndOfToken(PrevTokLocation);
  const char *Spelling = nullptr;
  if (EndLoc.isValid())
    Spelling = tok::getPunctuatorSpelling(ExpectedTok);

  DiagnosticBuilder DB =
      Spelling
          ? Diag(EndLoc, DiagID) << FixItHint::CreateInsertion(EndLoc, Spelling)
          : Diag(Tok, DiagID);
  if (DiagID == diag::err_expected)
    DB << ExpectedTok;
  else if (DiagID == diag::err_expected_after)
    DB << Msg << ExpectedTok;
  else
    DB << Msg;

  return true;
}

static bool HasFlagsSet(Parser::SkipUntilFlags L, Parser::SkipUntilFlags R) {
  return (static_cast<unsigned>(L) & static_cast<unsigned>(R)) != 0;
}

/// SkipUntil - Read tokens until we get to the specified token, then consume
/// it (unless no flag StopBeforeMatch).  Because we cannot guarantee that the
/// token will ever occur, this skips to the next token, or to some likely
/// good stopping point.  If StopAtSemi is true, skipping will stop at a ';'
/// character.
///
/// If SkipUntil finds the specified token, it returns true, otherwise it
/// returns false.
bool Parser::SkipUntil(ArrayRef<tok::TokenKind> Toks, SkipUntilFlags Flags) {
  // We always want this function to skip at least one token if the first token
  // isn't T and if not at EOF.
  bool isFirstTokenSkipped = true;
  while (true) {
    // If we found one of the tokens, stop and return true.
    for (unsigned i = 0, NumToks = Toks.size(); i != NumToks; ++i) {
      if (Tok.is(Toks[i])) {
        if (HasFlagsSet(Flags, StopBeforeMatch)) {
          // Noop, don't consume the token.
        } else {
          ConsumeAnyToken();
        }
        return true;
      }
    }

    // Important special case: The caller has given up and just wants us to
    // skip the rest of the file. Do this without recursing, since we can
    // get here precisely because the caller detected too much recursion.
    if (Toks.size() == 1 && Toks[0] == tok::eof &&
        !HasFlagsSet(Flags, StopAtSemi) &&
        !HasFlagsSet(Flags, StopAtCodeCompletion)) {
      while (Tok.isNot(tok::eof))
        ConsumeAnyToken();
      return true;
    }

    switch (Tok.getKind()) {
    case tok::eof:
      // Ran out of tokens.
      return false;

    case tok::code_completion:
      if (!HasFlagsSet(Flags, StopAtCodeCompletion))
        HandleUnexpectedCodeCompletionToken();
      return false;

    case tok::l_paren:
      // Recursively skip properly-nested parens.
      ConsumeParen();
      if (HasFlagsSet(Flags, StopAtCodeCompletion))
        SkipUntil(tok::r_paren, StopAtCodeCompletion);
      else
        SkipUntil(tok::r_paren);
      break;
    case tok::l_square:
      // Recursively skip properly-nested square brackets.
      ConsumeBracket();
      if (HasFlagsSet(Flags, StopAtCodeCompletion))
        SkipUntil(tok::r_square, StopAtCodeCompletion);
      else
        SkipUntil(tok::r_square);
      break;
    case tok::l_brace:
      // Recursively skip properly-nested braces.
      ConsumeBrace();
      if (HasFlagsSet(Flags, StopAtCodeCompletion))
        SkipUntil(tok::r_brace, StopAtCodeCompletion);
      else
        SkipUntil(tok::r_brace);
      break;
    case tok::question:
      // Recursively skip ? ... : pairs; these function as brackets. But
      // still stop at a semicolon if requested.
      ConsumeToken();
      SkipUntil(tok::colon,
                SkipUntilFlags(unsigned(Flags) &
                               unsigned(StopAtCodeCompletion | StopAtSemi)));
      break;

    // Okay, we found a ']' or '}' or ')', which we think should be balanced.
    // Since the user wasn't looking for this token (if they were, it would
    // already be handled), this isn't balanced.  If there is a LHS token at a
    // higher level, we will assume that this matches the unbalanced token
    // and return it.  Otherwise, this is a spurious RHS token, which we skip.
    case tok::r_paren:
      if (ParenCount && !isFirstTokenSkipped)
        return false; // Matches something.
      ConsumeParen();
      break;
    case tok::r_square:
      if (BracketCount && !isFirstTokenSkipped)
        return false; // Matches something.
      ConsumeBracket();
      break;
    case tok::r_brace:
      if (BraceCount && !isFirstTokenSkipped)
        return false; // Matches something.
      ConsumeBrace();
      break;
    case tok::semi:
      if (HasFlagsSet(Flags, StopAtSemi)) {
        return false;
      }
      [[fallthrough]];
    default:
      // Skip this token.
      ConsumeAnyToken();
      break;
    }
    isFirstTokenSkipped = false;
  }
}
