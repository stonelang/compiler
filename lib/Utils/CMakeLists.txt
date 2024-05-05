set(LLVM_LINK_COMPONENTS
  FrontendOpenMP
  MC
  MCParser
  Support
  TargetParser
  )

add_clang_library(clangUtils
  
  DiagnosticEngine.cpp

  LINK_LIBS
   
  clangBasic

  DEPENDS
  omp_gen
  )
