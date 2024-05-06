#include "clang/Parse/Parser.h"
#include "clang/Parse/Parsing.h"

using namespace clang;

ParserStatus Parser::CollectDeclSpec(ParsingDeclSpec &spec) {

  ParserStatus foundSpec;
  foundSpec = CollectAccessLevelSpec(spec);
  if (foundSpec.IsSuccess()) {
    return foundSpec;
  }
  foundSpec = CollectFunctionSpec(spec);
  if (foundSpec.IsSuccess()) {
    return foundSpec;
  }
  foundSpec = CollectBasicTypeSpec(spec);
  if (foundSpec.IsSuccess()) {
    return foundSpec;
  }
  foundSpec = CollectNominalTypeSpec(spec);
  if (foundSpec.IsSuccess()) {
    return foundSpec;
  }
  foundSpec = CollectQualTypeSpec(spec);
  if (foundSpec.IsSuccess()) {
    return foundSpec;
  }
  // status = CollectStorageSpec(collector);
  // if (status.IsSuccess()) {
  //   return status;
  // }
  // If we are here, we did not find anything
  foundSpec.SetHasCodeCompletion();
  return foundSpec;
}

ParserStatus Parser::CollectAccessLevelSpec(ParsingDeclSpec &spec) {
  switch (Tok.getKind()) {
  case tok::kw_public: {
    if (spec.setPublicSpec(ConsumeToken(), spec.prevSpec, spec.diagID)) {
      return clang::MakeParserError();
    }
    break;
  }
  case tok::kw_protected: {
    if (spec.setProtectedSpec(ConsumeToken(), spec.prevSpec, spec.diagID)) {
      return clang::MakeParserError();
    }
    break;
  }
  case tok::kw_private: {
    if (spec.setPrivateSpec(ConsumeToken(), spec.prevSpec, spec.diagID)) {
      return clang::MakeParserError();
    }
    break;
  }
  default:
    return clang::MakeParserCodeCompletionStatus();
  }
  return clang::MakeParserSuccess();
}

ParserStatus Parser::CollectFunctionSpec(ParsingDeclSpec &spec) {
  switch (Tok.getKind()) {
  case tok::kw_fun: {
    if (spec.setFunctionSpec(ConsumeToken(), spec.prevSpec, spec.diagID)) {
      return clang::MakeParserError();
    }
    break;
  }
  default:
    return clang::MakeParserCodeCompletionStatus();
  }
  return clang::MakeParserSuccess();
}

ParserStatus Parser::CollectQualTypeSpec(ParsingDeclSpec &spec) {
  switch (Tok.getKind()) {
  case tok::kw_const: {
    if (spec.SetTypeQual(DeclSpec::TQ_const, ConsumeToken(), spec.prevSpec,
                         spec.diagID, GetLangOpts())) {
      return clang::MakeParserError();
    }
    break;
  }
  default:
    return clang::MakeParserCodeCompletionStatus();
  }
  return clang::MakeParserSuccess();
}

ParserStatus Parser::CollectBasicTypeSpec(ParsingDeclSpec &spec) {
  switch (Tok.getKind()) {
  case tok::kw_int: {
    if (spec.SetTypeSpecType(DeclSpec::TST_int, ConsumeToken(), spec.prevSpec,
                             spec.diagID, GetLangOpts())) {
      return clang::MakeParserError();
    }
    break;
  }
  default:
    return clang::MakeParserCodeCompletionStatus();
  }
  return clang::MakeParserSuccess();
}

ParserStatus Parser::CollectNominalTypeSpec(ParsingDeclSpec &spec) {
  switch (Tok.getKind()) {
  case tok::kw_struct: {
    if (spec.SetTypeSpecType(DeclSpec::TST_struct, ConsumeToken(),
                             spec.prevSpec, spec.diagID,
                             GetSema().getPrintingPolicy())) {
      return clang::MakeParserError();
    }
    break;
  }
  case tok::kw_enum: {
    if (spec.SetTypeSpecType(DeclSpec::TST_enum, ConsumeToken(), spec.prevSpec,
                             spec.diagID, GetSema().getPrintingPolicy())) {
      return clang::MakeParserError();
    }
    break;
  }
  case tok::kw_class: {
    if (spec.SetTypeSpecType(DeclSpec::TST_class, ConsumeToken(), spec.prevSpec,
                             spec.diagID, GetSema().getPrintingPolicy())) {
      return clang::MakeParserError();
    }
    break;
  }
  default:
    return clang::MakeParserCodeCompletionStatus();
  }
  return clang::MakeParserSuccess();
}