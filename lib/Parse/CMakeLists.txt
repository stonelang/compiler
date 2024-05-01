set(LLVM_LINK_COMPONENTS
  FrontendOpenMP
  MC
  MCParser
  Support
  TargetParser
  )

add_clang_library(clangParse
  
  CollectDeclSpec.cpp
  IdentifierInfoCache.cpp
  ParseDecl.cpp
  ParseExpr.cpp
  Parser.cpp
  ParseAST.cpp
  ParseStmt.cpp
  ParseTemplate.cpp
  ParseType.cpp
  ParseDeclarator.cpp

  LINK_LIBS
  clangAST
  clangBasic
  clangLex
  clangSema

  DEPENDS
  omp_gen
  )
