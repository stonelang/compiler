#include "clang/Parse/Parser.h"
#include "clang/Parse/Parsing.h"

using namespace clang;

bool Parser::IsTopLevelDeclSpec() {
  switch (Tok.getKind()) {
  case tok::kw_fun:
  case tok::kw_enum:
  case tok::kw_public:
  case tok::kw_class:
  case tok::kw_interface:
    return true;
  default:
    return false;
  }
}

void Parser::ParseTopLevelDecls(
    llvm::SmallVector<ParserResult<Decl>> &results) {

  auto Success = [&](ParserResult<Decl> &result) -> bool {
    return (!result.IsError() && result.IsNonNull());
  };
  while (IsParsing() && IsTopLevelDeclSpec()) {
    ParsingDeclSpec spec(*this);
    spec.isTopLevelDecl = true;
    auto result = ParseTopLevelDecl(spec);
    if (!Success(result)) {
      return;
    }
    results.push_back(result);
  }
}

ParserResult<Decl> Parser::ParseTopLevelDecl(ParsingDeclSpec &spec) {

  ParserResult<Decl> result;
  ParsedAttributes attrs(attrFactory);
  ParsedAttributes declSpecAttrs(attrFactory);

  assert(spec.getSourceRange().isInvalid() &&
         "expected uninitialised source range");

  spec.SetRangeStart(attrs.Range.getBegin());
  spec.SetRangeEnd(declSpecAttrs.Range.getEnd());
  spec.takeAttributesFrom(declSpecAttrs);

  while (result.IsNull() && IsParsing() && IsTopLevelDeclSpec()) {
    // Try to collect a decl spec
    if (CollectDeclSpec(spec).HasCodeCompletion()) {
      return result;
    }
    result = ParseDecl(DeclaratorContext::File, attrs, declSpecAttrs, spec);
  }
  return result;
}

ParserResult<Decl> Parser::ParseDecl(DeclaratorContext declaratorContext,
                                     ParsedAttributes &attrs,
                                     ParsedAttributes &declSpecAttrs,
                                     ParsingDeclSpec &spec) {

  // Now that we have a valide decl spec, get its declarator
  ParsedAttributes localAttrs(attrFactory);
  localAttrs.takeAllFrom(attrs);

  ParsingDeclarator declarator(*this, spec, localAttrs, declaratorContext);
  ParseDeclarator(declarator);

  // Bail out if the first declarator didn't seem well-formed.
  if (!declarator.hasName() && !declarator.mayOmitIdentifier()) {
    // SkipMalformedDecl();
    return ParserResult<Decl>();
  }

  // Ok, we have found a declarator for the decl spec -- parse the decl spec

  if (spec.isFunSpecified()) {
    return ParseFunDecl(declarator);
  } else if (spec.hasStructSpecifier()) {
    return ParseStructDecl(declarator);
  }

  else if (spec.hasEnumSpecifier()) {
    return ParseEnumDecl(declarator);
  }

  else if (spec.hasStructSpecifier()) {
    return ParseClassDecl(declarator);
  }

  return ParserResult<Decl>();
}

ParserResult<Decl> Parser::ParseFunDecl(ParsingDeclarator &declarator) {
  ParserResult<Decl> result;

  assert(declarator.getDeclSpec().isFunSpecified());
  assert(declarator.isFunctionDeclarator());

  ParsingScope funDeclScope(this, Scope::FnScope | Scope::DeclScope);

  return result;
}

ParserResult<Decl> Parser::ParseEnumDecl(ParsingDeclarator &declarator) {
  ParserResult<Decl> result;

  assert(declarator.getDeclSpec().hasEnumSpecifier());

  ParsingScope funDeclScope(this, Scope::EnumScope | Scope::DeclScope);

  // ParseFunctionDeclarator();

  return result;
}

ParserResult<Decl> Parser::ParseStructDecl(ParsingDeclarator &declarator) {
  ParserResult<Decl> result;

  assert(declarator.getDeclSpec().hasStructSpecifier());

  ParsingScope structDeclScope(this, Scope::ClassScope | Scope::DeclScope);

  return result;
}

void Parser::ParseStructDeclMembers(Decl *structDecl) {

  // Enter a scope for the class.
  ParsingScope classScope(this, Scope::ClassScope | Scope::DeclScope);

  // Note that we are parsing a new (potentially-nested) class definition.
  // ParsingClassDefinition ParsingDef(*this, TagDecl, NonNestedClass,
  //                                   TagType == DeclSpec::TST_interface);
  // if (structDecl){
  //   sema.ActOnStructStartDefinition(GetCurScope(), classDecl);
  // }
}

ParserResult<Decl> Parser::ParseClassDecl(ParsingDeclarator &declarator) {
  ParserResult<Decl> result;

  assert(declarator.getDeclSpec().hasClassSpecifier());

  ParsingScope classDeclScope(this, Scope::ClassScope | Scope::DeclScope);

  return result;
}

void Parser::ParseClassDeclMembers(Decl *classDecl) {

  // Enter a scope for the class.
  ParsingScope classScope(this, Scope::ClassScope | Scope::DeclScope);

  // Note that we are parsing a new (potentially-nested) class definition.
  // ParsingClassDefinition ParsingDef(*this, TagDecl, NonNestedClass,
  //                                   TagType == DeclSpec::TST_interface);

  // if (classDecl){
  //   sema.ActOnClassStartDefinition(GetCurScope(), classDecl);
  // }
}

ParserResult<Decl>
Parser::ParseImportDecl(ParsingDeclSpec &spec,
                        Sema::ModuleImportState &importState) {

  ParserResult<Decl> result;

  return result;
}
