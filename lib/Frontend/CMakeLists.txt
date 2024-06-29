set(LLVM_LINK_COMPONENTS
  Support
  TargetParser
  FrontendOpenMP
)

add_clang_library(clangFrontend
FrontendInstance.cpp   

)

