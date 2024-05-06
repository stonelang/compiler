set(LLVM_COMMON_DEPENDS_OLD ${LLVM_COMMON_DEPENDS})

# Drop clang-tablegen-targets from LLVM_COMMON_DEPENDS.
# so that we could use clangSupport within clang-tblgen and other clang
# component.
list(REMOVE_ITEM LLVM_COMMON_DEPENDS clang-tablegen-targets)

set(LLVM_LINK_COMPONENTS
  Support
  )

set(clang_gen_sources
  Sem.cpp
)

add_clang_library(clangSem ${clang_gen_sources})