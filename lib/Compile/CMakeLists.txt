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
  CodeGen.cpp
  Compile.cpp

  DEPENDS
  ClangDriverOptions

  LINK_LIBS
  ${compile_link_libs}

)
