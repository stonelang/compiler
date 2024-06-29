set(LLVM_LINK_COMPONENTS
  Support
  TargetParser
  FrontendOpenMP
)

add_clang_library(clangCompile
Compile.cpp
CompilerInstance.cpp   

)

