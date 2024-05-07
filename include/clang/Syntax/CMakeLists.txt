clang_tablegen(TypeNode.inc -gen-clang-type-nodes
  SOURCE TypeNode.td TARGET ClangTypeNode)

clang_tablegen(DeclNode.inc -gen-clang-decl-nodes
  SOURCE DeclNode.td TARGET ClangDeclNode)