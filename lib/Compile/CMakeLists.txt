set(LLVM_LINK_COMPONENTS
  Option
  Support
  )

set(compile_link_libs
  clangBasic
  clangCodeGen
  clangDriver
  clangExtractAPI
  clangFrontend
  clangRewriteFrontend

   clangCodeGeneration
  )

add_clang_library(clangCompile

  Compile.cpp


  CollectDeclSpec.cpp
  IdentifierInfoCache.cpp
  Lexer.cpp
  ParseDecl.cpp
  ParseExpr.cpp
  Parser.cpp
  ParseAST.cpp
  ParseStmt.cpp
  ParseTemplate.cpp
  ParseType.cpp
  ParseDeclarator.cpp
  Parsing.cpp


  DeclSpec.cpp
  Sem.cpp
  SemDecl.cpp




  DEPENDS
  ClangDriverOptions

  LINK_LIBS
  ${compile_link_libs}

)
