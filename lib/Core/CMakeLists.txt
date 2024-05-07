set(LLVM_LINK_COMPONENTS
  FrontendOpenMP
  MC
  MCParser
  Support
  TargetParser
)

add_clang_library(clangCore
  
  Diagnostics.cpp

  LINK_LIBS
  clangBasic

  DEPENDS
  
)
