set(LLVM_LINK_COMPONENTS
  Option
  Support
  )

set(link_libs
  clangBasic
  clangCodeGen
  clangDriver
  clangExtractAPI
  clangFrontend
  clangRewriteFrontend
  )

add_clang_library(clangCompile
  CodeGen.cpp
  Compile.cpp

  DEPENDS
  ClangDriverOptions

  LINK_LIBS
  ${link_libs}


)
