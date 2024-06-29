#ifndef LLVM_CLANG_FRONTEND_FRONTENDOPTIONS_H
#define LLVM_CLANG_FRONTEND_FRONTENDOPTIONS_H

namespace clang {

enum class FrontendAction {
  None = 0,
  PrintVersion,
  PrintHelp,
  PrintHelpHidden,
  PrintFeature,
  Parse,
  PrintAST, /// < Print AST without type-checking
  ResolveImports,
  TypeCheck,
  EmitAST, /// < Emit AST without type-checkint
  PrintIR, /// < Print IR without optimization
  EmitIR,  /// < Emit IR after optimization
  EmitBC,
  EmitObject,
  EmitModule,
  MergeModules,
  EmitAssembly,

};

class FrontendInputFile final {
public:
};

class FrontendOptions final {

public:
  FrontendAction mainAction = FrontendAction::None;
};

} // namespace clang

#endif