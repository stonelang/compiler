#ifndef LLVM_CLANG_COMPILE_COMPILEROPTIONS_H
#define LLVM_CLANG_COMPILE_COMPILEROPTIONS_H

namespace clang {

enum class CompilerActionKind {
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

class CompilerOptions final {

public:
  CompilerActionKind mainActionKind = CompilerActionKind::None;
};

} // namespace clang

#endif