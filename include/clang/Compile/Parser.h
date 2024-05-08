#ifndef LLVM_CLANG_FRONTNED_PARSER_H
#define LLVM_CLANG_FRONTNED_PARSER_H

#include "clang/AST/AST.h"
#include "clang/AST/Availability.h"
#include "clang/Basic/BitmaskEnum.h"
#include "clang/Basic/OpenMPKinds.h"
#include "clang/Basic/OperatorPrecedence.h"
#include "clang/Basic/Specifiers.h"
#include "clang/Basic/TokenKinds.h"
#include "clang/Lex/CodeCompletionHandler.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/IdentifierInfoCache.h"
#include "clang/Parse/ParserDiagnostic.h"
#include "clang/Parse/ParserLoopHint.h"
#include "clang/Parse/ParserResult.h"
#include "clang/Sema/DeclSpec.h"
#include "clang/Sema/Sema.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/SaveAndRestore.h"

#include <memory>
#include <optional>
#include <stack>

namespace clang {

class Scope;
class BalancedDelimiterTracker;
class CorrectionCandidateCallback;
class DeclGroupRef;
class DiagnosticBuilder;
struct ParserLoopHint;
class Parser;
class ParsingDeclSpec;
class ParsingDeclRAIIObject;
class ParsingDeclSpec;
class ParsingDeclarator;
class ParsingFieldDeclarator;
class ColonProtectionRAIIObject;

class ParserOptions final {
  bool skipFunctionBodies = false;
};

class Parser final : public CodeCompletionHandler {

  friend class ColonProtectionRAIIObject;
  friend class ParenBraceBracketBalancer;
  friend class BalancedDelimiterTracker;

  Preprocessor &lexer;

  /// Actions - These are the callbacks we invoke as we parse various constructs
  /// in the file.
  Sema &sema;

  DiagnosticsEngine &diags;

  /// Tok - The current token we are peeking ahead.  All parsing methods assume
  /// that this is valid.
  Token Tok;

  // PrevTokLocation - The location of the token we previously
  // consumed. This token is used for diagnostics where we expected to
  // see a token following another token (e.g., the ';' at the end of
  // a statement).
  SourceLocation PrevTokLocation;

  /// Tracks an expected type for the current token when parsing an expression.
  /// Used by code completion for ranking.
  PreferredTypeBuilder PreferredType;

  unsigned short ParenCount = 0;
  unsigned short BracketCount = 0;
  unsigned short BraceCount = 0;
  unsigned short MisplacedModuleBeginCount = 0;

  /// ScopeCache - Cache scopes to reduce malloc traffic.
  // enum { ScopeCacheSize = 16 };
  // unsigned NumCachedScopes;
  // Scope *ScopeCache[ScopeCacheSize];

  /// Whether the '>' token acts as an operator or not. This will be
  /// true except when we are parsing an expression within a C++
  /// template argument list, where the '>' closes the template
  /// argument list.
  bool GreaterThanIsOperator;

  /// ColonIsSacred - When this is false, we aggressively try to recover from
  /// code like "foo : bar" as if it were a typo for "foo :: bar".  This is not
  /// safe in case statements and a few other things.  This is managed by the
  /// ColonProtectionRAIIObject RAII object.
  bool ColonIsSacred;

  /// Factory object for creating ParsedAttr objects.
  AttributeFactory attrFactory;

  /// The "depth" of the template parameters currently being parsed.
  unsigned TemplateParameterDepth;

  IdentifierInfoCache identifierInfoCache;

  /// Tracker for '<' tokens that might have been intended to be treated as an
  /// angle bracket instead of a less-than comparison.
  ///
  /// This happens when the user intends to form a template-id, but typoes the
  /// template-name or forgets a 'template' keyword for a dependent template
  /// name.
  ///
  /// We track these locations from the point where we see a '<' with a
  /// name-like expression on its left until we see a '>' or '>>' that might
  /// match it.
  struct AngleBracketTracker {
    /// Flags used to rank candidate template names when there is more than one
    /// '<' in a scope.
    enum Priority : unsigned short {
      /// A non-dependent name that is a potential typo for a template name.
      PotentialTypo = 0x0,
      /// A dependent name that might instantiate to a template-name.
      DependentName = 0x2,

      /// A space appears before the '<' token.
      SpaceBeforeLess = 0x0,
      /// No space before the '<' token
      NoSpaceBeforeLess = 0x1,

      LLVM_MARK_AS_BITMASK_ENUM(/*LargestValue*/ DependentName)
    };
    struct Loc {
      Expr *TemplateName;
      SourceLocation LessLoc;
      AngleBracketTracker::Priority Priority;
      unsigned short ParenCount, BracketCount, BraceCount;

      bool isActive(Parser &P) const {
        return P.ParenCount == ParenCount && P.BracketCount == BracketCount &&
               P.BraceCount == BraceCount;
      }

      bool isActiveOrNested(Parser &P) const {
        return isActive(P) || P.ParenCount > ParenCount ||
               P.BracketCount > BracketCount || P.BraceCount > BraceCount;
      }
    };

    SmallVector<Loc, 8> Locs;

    /// Add an expression that might have been intended to be a template name.
    /// In the case of ambiguity, we arbitrarily select the innermost such
    /// expression, for example in 'foo < bar < baz', 'bar' is the current
    /// candidate. No attempt is made to track that 'foo' is also a candidate
    /// for the case where we see a second suspicious '>' token.
    void add(Parser &P, Expr *TemplateName, SourceLocation LessLoc,
             Priority Prio) {
      if (!Locs.empty() && Locs.back().isActive(P)) {
        if (Locs.back().Priority <= Prio) {
          Locs.back().TemplateName = TemplateName;
          Locs.back().LessLoc = LessLoc;
          Locs.back().Priority = Prio;
        }
      } else {
        Locs.push_back({TemplateName, LessLoc, Prio, P.ParenCount,
                        P.BracketCount, P.BraceCount});
      }
    }

    /// Mark the current potential missing template location as having been
    /// handled (this happens if we pass a "corresponding" '>' or '>>' token
    /// or leave a bracket scope).
    void clear(Parser &P) {
      while (!Locs.empty() && Locs.back().isActiveOrNested(P))
        Locs.pop_back();
    }

    /// Get the current enclosing expression that might hve been intended to be
    /// a template name.
    Loc *getCurrent(Parser &P) {
      if (!Locs.empty() && Locs.back().isActive(P))
        return &Locs.back();
      return nullptr;
    }
  };

  AngleBracketTracker AngleBrackets;

public:
  Parser(Sema &sema, bool skipFunctionBodies);
  ~Parser() override;

public:
  DiagnosticBuilder Diag(SourceLocation Loc, unsigned DiagID);
  DiagnosticBuilder Diag(const Token &Tok, unsigned DiagID);
  DiagnosticBuilder Diag(unsigned DiagID) { return Diag(Tok, DiagID); }

public:
  // Type forwarding.  All of these are statically 'void*', but they may all be
  // different actual classes based on the actions in place.
  using DeclGroupPtrTy = OpaquePtr<DeclGroupRef>;
  using TemplateTy = OpaquePtr<TemplateName>;

public:
  /// Control flags for SkipUntil functions.
  enum SkipUntilFlags {
    StopAtSemi = 1 << 0, ///< Stop skipping at semicolon
    /// Stop skipping at specified token, but don't skip the token itself
    StopBeforeMatch = 1 << 1,
    StopAtCodeCompletion = 1 << 2 ///< Stop at code completion
  };

  friend constexpr SkipUntilFlags operator|(SkipUntilFlags L,
                                            SkipUntilFlags R) {
    return static_cast<SkipUntilFlags>(static_cast<unsigned>(L) |
                                       static_cast<unsigned>(R));
  }

  /// SkipUntil - Read tokens until we get to the specified token, then consume
  /// it (unless StopBeforeMatch is specified).  Because we cannot guarantee
  /// that the token will ever occur, this skips to the next token, or to some
  /// likely good stopping point.  If Flags has StopAtSemi flag, skipping will
  /// stop at a ';' character. Balances (), [], and {} delimiter tokens while
  /// skipping.
  ///
  /// If SkipUntil finds the specified token, it returns true, otherwise it
  /// returns false.
  bool SkipUntil(tok::TokenKind T,
                 SkipUntilFlags Flags = static_cast<SkipUntilFlags>(0)) {
    return SkipUntil(llvm::ArrayRef(T), Flags);
  }
  bool SkipUntil(tok::TokenKind T1, tok::TokenKind T2,
                 SkipUntilFlags Flags = static_cast<SkipUntilFlags>(0)) {
    tok::TokenKind TokArray[] = {T1, T2};
    return SkipUntil(TokArray, Flags);
  }
  bool SkipUntil(tok::TokenKind T1, tok::TokenKind T2, tok::TokenKind T3,
                 SkipUntilFlags Flags = static_cast<SkipUntilFlags>(0)) {
    tok::TokenKind TokArray[] = {T1, T2, T3};
    return SkipUntil(TokArray, Flags);
  }
  bool SkipUntil(ArrayRef<tok::TokenKind> Toks,
                 SkipUntilFlags Flags = static_cast<SkipUntilFlags>(0));

  /// SkipMalformedDecl - Read tokens until we get to some likely good stopping
  /// point for skipping past a simple-declaration.
  void SkipMalformedDecl();

public:
  Preprocessor &GetLexer() { return lexer; }
  const LangOptions &GetLangOpts() const { return lexer.getLangOpts(); }
  const TargetInfo &GetTargetInfo() const { return lexer.getTargetInfo(); }
  Sema &GetSema() const { return sema; }
  AttributeFactory &GetAttrFactory() { return attrFactory; }
  Token &GetTok() { return Tok; }

  IdentifierInfoCache &GetIdentifierInfoCache() { return identifierInfoCache; }
  IdentifierInfo *GetIdentifierInfo(StringRef name) {
    return &GetLexer().getIdentifierTable().get(name);
  }

public:
  void EndParsing() { CutOffParsing(); }
  bool IsEOF() { return Tok.getKind() == tok::eof; }
  bool IsParsing() { return !IsEOF() && !diags.hasErrorOccurred(); }

public:
  ParserStatus CollectDeclSpec(ParsingDeclSpec &spec);
  ParserStatus CollectAccessLevelSpec(ParsingDeclSpec &spec);
  ParserStatus CollectQualTypeSpec(ParsingDeclSpec &spec);
  ParserStatus CollectFunctionSpec(ParsingDeclSpec &spec);
  ParserStatus CollectBasicTypeSpec(ParsingDeclSpec &spec);
  ParserStatus CollectNominalTypeSpec(ParsingDeclSpec &spec);

public:
  bool IsTopLevelDeclSpec();
  void ParseTopLevelDecls(llvm::SmallVector<ParserResult<Decl>> &results);
  ParserResult<Decl> ParseTopLevelDecl(ParsingDeclSpec &spec);

  ParserResult<Decl> ParseDecl(DeclaratorContext declaratorContext,
                               ParsedAttributes &attrs,
                               ParsedAttributes &declSpecAttrs,
                               ParsingDeclSpec &spec);

  ParserResult<Decl> ParseImportDecl(ParsingDeclSpec &spec,
                                     Sema::ModuleImportState &importState);

  //== fun ==//
  // ParserResult<Decl> ParseFunDecl(DeclaratorContext declaratorContext,
  //                                 ParsingDeclSpec &spec);

  ParserResult<Decl> ParseFunDecl(ParsingDeclarator &declarator);

  void ParseFunctionDeclarator(Declarator &D, ParsedAttributes &firstArgAttrs,
                               BalancedDelimiterTracker &Tracker,
                               bool isAmbiguous, bool requiresArg = false);

  ParserResult<Decl> ParseStructDecl(ParsingDeclarator &declarator);
  void ParseStructDeclMembers(Decl *structDecl);

  ParserResult<Decl> ParseEnumDecl(ParsingDeclarator &declarator);
  ParserResult<Decl> ParseEnumDeclMembers(ParsingDeclarator &declarator);

  ParserResult<Decl> ParseClassDecl(ParsingDeclarator &declarator);
  void ParseClassDeclMembers(Decl *classDecl);

  void ParseDeclarator(Declarator &declarator);

  /// DeclaratorScopeObj - RAII object used in Parser::ParseDirectDeclarator to
  /// enter a new C++ declarator scope and exit it when the function is
  /// finished.
  class DirectDeclaratorScope final {
    Parser &P;
    CXXScopeSpec &SS;
    bool EnteredScope;
    bool CreatedScope;

  public:
    DirectDeclaratorScope(Parser &p, CXXScopeSpec &ss)
        : P(p), SS(ss), EnteredScope(false), CreatedScope(false) {}

    void EnterScope() {
      assert(!EnteredScope && "Already entered the scope!");
      assert(SS.isSet() && "C++ scope was not set!");

      CreatedScope = true;
      P.EnterScope(0); // Not a decl scope.

      if (!P.GetSema().ActOnCXXEnterDeclaratorScope(P.GetCurScope(), SS))
        EnteredScope = true;
    }

    ~DirectDeclaratorScope() {
      if (EnteredScope) {
        assert(SS.isSet() && "C++ scope was cleared ?");
        P.GetSema().ActOnCXXExitDeclaratorScope(P.GetCurScope(), SS);
      }
      if (CreatedScope)
        P.ExitScope();
    }
  };
  void ParseDirectDeclarator(Declarator &declarator);
  void ParseBracketDeclarator(Declarator &declarator);
  void ParseParenDeclarator(Declarator &declarator);

public:
  ParserResult<QualType> ParseType(ParsingDeclSpec &spec);
  ParserResult<QualType> ParseBasicType(ParsingDeclSpec &spec);

public:
  //  /// TypeCastState - State whether an expression is or may be a type cast.
  // enum class TypeCastState {
  //   None = 0,
  //   Maybe,
  //   Is
  // };

  //  ParserResult<Expr> ParseExpr(TypeCastState typeCastState =
  //  TypeCastState::None);

public:
  //===--------------------------------------------------------------------===//
  // Scope manipulation

  using ScopeCache = llvm::SmallVector<Scope *, 16>;
  ScopeCache scopeCache;

  /// ParseScope - Introduces a new scope for parsing. The kind of
  /// scope is determined by ScopeFlags. Objects of this type should
  /// be created on the stack to coincide with the position where the
  /// parser enters the new scope, and this object's constructor will
  /// create that new scope. Similarly, once the object is destroyed
  /// the parser will exit the scope.
  class ParsingScope final {
    Parser *Self;
    ParsingScope(const ParsingScope &) = delete;
    void operator=(const ParsingScope &) = delete;

  public:
    // ParseScope - Construct a new object to manage a scope in the
    // parser Self where the new Scope is created with the flags
    // ScopeFlags, but only when we aren't about to enter a compound statement.
    ParsingScope(Parser *Self, unsigned ScopeFlags, bool EnteredScope = true,
                 bool BeforeCompoundStmt = false)
        : Self(Self) {
      if (EnteredScope && !BeforeCompoundStmt) {
        Self->EnterScope(ScopeFlags);
      } else {
        if (BeforeCompoundStmt) {
          // TODO: Self->incrementMSManglingNumber();
        }
        this->Self = nullptr;
      }
    }
    // Exit - Exit the scope associated with this object now, rather
    // than waiting until the object is destroyed.
    void Exit() {
      if (Self) {
        Self->ExitScope();
        Self = nullptr;
      }
    }
    ~ParsingScope() { Exit(); }
  };

  /// Introduces zero or more scopes for parsing. The scopes will all be exited
  /// when the object is destroyed.
  class MultiParsingScope {
    Parser &Self;
    unsigned NumScopes = 0;

    MultiParsingScope(const MultiParsingScope &) = delete;

  public:
    MultiParsingScope(Parser &Self) : Self(Self) {}
    void Enter(unsigned ScopeFlags) {
      Self.EnterScope(ScopeFlags);
      ++NumScopes;
    }
    void Exit() {
      while (NumScopes) {
        Self.ExitScope();
        --NumScopes;
      }
    }
    ~MultiParsingScope() { Exit(); }
  };

  /// HasCurScope
  bool HasCurScope() const { return (scopeCache.size() > 0 && GetCurScope()); }

  /// Pop the latest scope
  void PopCurScope() { scopeCache.pop_back(); }

  /// Push the current scope
  void PushCurScope(Scope *scope);

  /// EnterScope - Start a new scope.
  void EnterScope(unsigned scopeFlags);

  /// ExitScope - Pop a scope off the scope stack.
  void ExitScope();

  /// CreateScope -- Create a new scope
  Scope *CreateScope(unsigned scopeFlags);

  /// GetCurScope
  Scope *GetCurScope() const { return sema.getCurScope(); }

private:
  /// RAII object used to modify the scope flags for the current scope.
  class ParsingScopeFlags {
    Scope *CurScope;
    unsigned OldFlags = 0;
    ParsingScopeFlags(const ParsingScopeFlags &) = delete;
    void operator=(const ParsingScopeFlags &) = delete;

  public:
    ParsingScopeFlags(Parser *Self, unsigned ScopeFlags,
                      bool ManageFlags = true);
    ~ParsingScopeFlags();
  };

public:
  /// ConsumeToken - Consume the current 'peek token' and lex the next one.
  /// This does not work with special tokens: string literals, code completion,
  /// annotation tokens and balanced tokens must be handled using the specific
  /// consume methods.
  /// Returns the location of the consumed token.
  SourceLocation ConsumeToken() {
    assert(!isTokenSpecial() &&
           "Should consume special tokens with Consume*Token");
    PrevTokLocation = Tok.getLocation();
    lexer.Lex(Tok);
    return PrevTokLocation;
  }

  bool TryConsumeToken(tok::TokenKind Expected) {
    if (Tok.isNot(Expected)) {
      return false;
    }
    assert(!isTokenSpecial() &&
           "Should consume special tokens with Consume*Token");
    PrevTokLocation = Tok.getLocation();
    lexer.Lex(Tok);
    return true;
  }

  bool TryConsumeToken(tok::TokenKind Expected, SourceLocation &Loc) {
    if (!TryConsumeToken(Expected)) {
      return false;
    }
    Loc = PrevTokLocation;
    return true;
  }

  /// ConsumeAnyToken - Dispatch to the right Consume* method based on the
  /// current token type.  This should only be used in cases where the type of
  /// the token really isn't known, e.g. in error recovery.
  SourceLocation ConsumeAnyToken(bool consumeCodeCompletionTok = false) {

    if (isTokenParen()) {
      return ConsumeParen();
    }
    if (isTokenBracket()) {
      return ConsumeBracket();
    }
    if (isTokenBrace()) {
      return ConsumeBrace();
    }
    if (isTokenStringLiteral()) {
      return ConsumeStringToken();
    }
    if (Tok.IsCodeCompletion()) {
      return consumeCodeCompletionTok ? ConsumeCodeCompletionToken()
                                      : HandleUnexpectedCodeCompletionToken();
    }
    if (Tok.isAnnotation()) {
      return ConsumeAnnotationToken();
    }
    return ConsumeToken();
  }

  /// ConsumeStringToken - Consume the current 'peek token', lexing a new one
  /// and returning the token kind.  This method is specific to strings, as it
  /// handles string literal concatenation, as per C99 5.1.1.2, translation
  /// phase #6.
  // SourceLocation ConsumeStringToken() {
  //   assert(isTokenStringLiteral() &&
  //          "Should only consume string literals with this method");
  //   PrevTokLocation = Tok.getLocation();
  //   lexer.Lex(Tok);
  //   return PrevTokLocation;
  // }

  ///\ brief When we are consuming a code-completion token without having
  /// matched specific position in the grammar, provide code-completion results
  /// based on context.
  ///
  /// \returns the source location of the code-completion token.
  SourceLocation HandleUnexpectedCodeCompletionToken();

  /// ExpectAndConsume - The parser expects that 'ExpectedTok' is next in the
  /// input.  If so, it is consumed and false is returned.
  ///
  /// If a trivial punctuator misspelling is encountered, a FixIt error
  /// diagnostic is issued and false is returned after recovery.
  ///
  /// If the input is malformed, this emits the specified diagnostic and true is
  /// returned.
  bool ExpectAndConsume(tok::TokenKind ExpectedTok,
                        unsigned Diag = diag::err_expected,
                        StringRef DiagMsg = "");

private:
  //===--------------------------------------------------------------------===//
  // Low-Level token peeking and consumption methods.
  //
  /// isTokenParen - Return true if the cur token is '(' or ')'.
  bool isTokenParen() const { return Tok.isOneOf(tok::l_paren, tok::r_paren); }
  /// isTokenBracket - Return true if the cur token is '[' or ']'.
  bool isTokenBracket() const {
    return Tok.isOneOf(tok::l_square, tok::r_square);
  }
  /// isTokenBrace - Return true if the cur token is '{' or '}'.
  bool isTokenBrace() const { return Tok.isOneOf(tok::l_brace, tok::r_brace); }
  /// isTokenStringLiteral - True if this token is a string-literal.
  bool isTokenStringLiteral() const {
    return tok::isStringLiteral(Tok.getKind());
  }
  /// isTokenSpecial - True if this token requires special consumption methods.
  bool isTokenSpecial() const {
    return isTokenStringLiteral() || isTokenParen() || isTokenBracket() ||
           isTokenBrace() || Tok.is(tok::code_completion) || Tok.isAnnotation();
  }

  /// Returns true if the current token is '=' or is a type of '='.
  /// For typos, give a fixit to '='
  bool isTokenEqualOrEqualTypo();

  /// Return the current token to the token stream and make the given
  /// token the current token.
  void UnconsumeToken(Token &Consumed) {
    Token Next = Tok;
    lexer.EnterToken(Consumed, /*IsReinject*/ true);
    lexer.Lex(Tok);
    lexer.EnterToken(Next, /*IsReinject*/ true);
  }

  SourceLocation ConsumeAnnotationToken() {
    assert(Tok.isAnnotation() && "wrong consume method");
    SourceLocation Loc = Tok.getLocation();
    PrevTokLocation = Tok.getAnnotationEndLoc();
    lexer.Lex(Tok);
    return Loc;
  }

  /// ConsumeParen - This consume method keeps the paren count up-to-date.
  ///
  SourceLocation ConsumeParen() {
    assert(isTokenParen() && "wrong consume method");
    if (Tok.getKind() == tok::l_paren)
      ++ParenCount;
    else if (ParenCount) {
      AngleBrackets.clear(*this);
      --ParenCount; // Don't let unbalanced )'s drive the count negative.
    }
    PrevTokLocation = Tok.getLocation();
    lexer.Lex(Tok);
    return PrevTokLocation;
  }

  /// ConsumeBracket - This consume method keeps the bracket count up-to-date.
  ///
  SourceLocation ConsumeBracket() {
    assert(isTokenBracket() && "wrong consume method");
    if (Tok.getKind() == tok::l_square)
      ++BracketCount;
    else if (BracketCount) {
      AngleBrackets.clear(*this);
      --BracketCount; // Don't let unbalanced ]'s drive the count negative.
    }

    PrevTokLocation = Tok.getLocation();
    lexer.Lex(Tok);
    return PrevTokLocation;
  }

  /// ConsumeBrace - This consume method keeps the brace count up-to-date.
  ///
  SourceLocation ConsumeBrace() {
    assert(isTokenBrace() && "wrong consume method");
    if (Tok.getKind() == tok::l_brace)
      ++BraceCount;
    else if (BraceCount) {
      AngleBrackets.clear(*this);
      --BraceCount; // Don't let unbalanced }'s drive the count negative.
    }

    PrevTokLocation = Tok.getLocation();
    lexer.Lex(Tok);
    return PrevTokLocation;
  }

  /// ConsumeStringToken - Consume the current 'peek token', lexing a new one
  /// and returning the token kind.  This method is specific to strings, as it
  /// handles string literal concatenation, as per C99 5.1.1.2, translation
  /// phase #6.
  SourceLocation ConsumeStringToken() {
    assert(isTokenStringLiteral() &&
           "Should only consume string literals with this method");
    PrevTokLocation = Tok.getLocation();
    lexer.Lex(Tok);
    return PrevTokLocation;
  }

  /// PeekNextToken - This peeks ahead one token and returns it without
  /// consuming it.
  const Token &PeekNextToken() { return lexer.LookAhead(0); }
  /// Consume the current code-completion token.
  ///
  /// This routine can be called to consume the code-completion token and
  /// continue processing in special cases where \c cutOffParsing() isn't
  /// desired, such as token caching or completion with lookahead.
  SourceLocation ConsumeCodeCompletionToken() {
    assert(Tok.is(tok::code_completion));
    PrevTokLocation = Tok.getLocation();
    lexer.Lex(Tok);
    return PrevTokLocation;
  }

  /// Abruptly cut off parsing; mainly used when we have reached the
  /// code-completion point.
  void CutOffParsing() {
    if (lexer.isCodeCompletionEnabled())
      lexer.setCodeCompletionReached();
    // Cut off parsing by acting as if we reached the end-of-file.
    Tok.setKind(tok::eof);
  }

  /// Determine if we're at the end of the file or at a transition
  /// between modules.
  bool isEofOrEom() {
    tok::TokenKind Kind = Tok.getKind();
    return Kind == tok::eof || Kind == tok::annot_module_begin ||
           Kind == tok::annot_module_end || Kind == tok::annot_module_include ||
           Kind == tok::annot_repl_input_end;
  }

  /// Checks if the \p Level is valid for use in a fold expression.
  bool isFoldOperator(prec::Level Level) const;

  /// Checks if the \p Kind is a valid operator for fold expressions.
  bool isFoldOperator(tok::TokenKind Kind) const;
};

} // namespace clang

#endif
