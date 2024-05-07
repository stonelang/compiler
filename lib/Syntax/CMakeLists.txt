set(LLVM_LINK_COMPONENTS
  Option
  Support
)

set(syntax_link_libs
  clangBasic
)

add_clang_library(clangSyntax
  
  CanType.cpp
  ASTContext.cpp
  Type.cpp
  Decl.cpp
 
  DEPENDS
  
  LINK_LIBS
  ${syntax_link_libs}

)
