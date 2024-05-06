set(LLVM_LINK_COMPONENTS
  Option
  Support
  )

set(codegen_link_libs
  clangBasic
  
)

add_clang_library(clangCodeGeneration
  CodeGen.cpp

  DEPENDS
  #ClangDriverOptions

  LINK_LIBS
  ${codegen_link_libs}

)
