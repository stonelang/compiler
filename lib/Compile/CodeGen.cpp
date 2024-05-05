

CodeGenAction::CodeGenAction(CodeGenKind kind)
    : kind(kind), codeGenModule(*this) {}

std::unique_ptr<ASTConsumer>
CodeGenAction::CreateASTConsumer(CompilerInstance &CI, StringRef InFile) {}