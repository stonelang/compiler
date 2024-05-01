#include "clang/Parse/Parser.h"
#include "clang/Parse/ParserRAII.h"

using namespace clang;

ParserStatus Parser::CollectDeclSpec(ParsingDeclSpec &spec) {

  ParserStatus status;
  status = CollectAccessLevelSpec(spec);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectFunctionSpec(spec);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectBasicTypeSpec(spec);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectNominalTypeSpec(spec);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectQualTypeSpec(spec);
  if (status.IsSuccess()) {
    return status;
  }
  // status = CollectStorageSpec(collector);
  // if (status.IsSuccess()) {
  //   return status;
  // }
  // If we are here, we did not find anything
  status.SetHasCodeCompletion();
  return status;
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

void ParsingDeclSpec::SetFunSpec(SourceLocation loc) {}

void ParsingDeclSpec::SetStructSpec(SourceLocation loc) {}

void ParsingDeclSpec::SetIntSpec(SourceLocation loc) {}

void ParsingDeclSpec::SetVoidSpec(SourceLocation loc) {}

void ParsingDeclSpec::SetBoolSpec(SourceLocation loc) {}

void ParsingDeclSpec::SetCharSpec(SourceLocation loc) {}
