#ifndef LLVM_CLANG_SYNTAX_DECL_H
#define LLVM_CLANG_SYNTAX_DECL_H

#include "clang/Core/InlineBitfield.h"
#include "clang/Syntax/ASTAllocation.h"
#include "clang/Syntax/TypeAlignment.h"

#include "llvm/ADT/ArrayRef.h"

#include <memory>

namespace clang {
namespace syntax {

enum class DeclKind : uint8_t {
#define DECL(DERIVED, BASE) DERIVED,
#define ABSTRACT_DECL(DECL)
#define DECL_RANGE(BASE, START, END) First##BASE = START, Last##BASE = END,
#define LAST_DECL_RANGE(BASE, START, END) First##BASE = START, Last##BASE = END
#include "clang/Syntax/DeclNode.inc"
};

enum : unsigned {
  NumDeclKindBits =
      clang::CountBitsUsed(static_cast<unsigned>(DeclKind::LastDecl))
};

class alignas(1 << DeclAlignInBits) Decl : public ASTAllocation<Decl> {
protected:
  DeclKind kind;

protected:
  union {
    uint64_t OpaqueBits;
    CLANG_INLINE_BITFIELD_BASE(
        Decl, clang::BitMax(NumDeclKindBits, 8) + 1 + 1 + 1 + 1 + 1 + 1, Kind
        : clang::BitMax(NumDeclKindBits, 8),

          /// Whether this declaration is invalid.
          IsValid : 1,

          /// Whether this declaration was implicitly created, e.g.,
          /// an implicit constructor in a struct.
          IsImplicit : 1,

          /// Whether this declaration was mapped directly from a Clang AST.
          ///
          /// Use GetClangNode() to retrieve the corresponding Clang AST.
          IsFromClang : 1,

          /// Whether this declaration was added to the surrounding
          /// DeclContext of an active #if config clause.
          Refere : 1,

          /// Whether this declaration is syntactically scoped inside of
          /// a local context, but should behave like a top-level
          /// declaration for name lookup purposes. This is used by
          /// lldb.
          Used : 1,

          /// Wether this is a top level decl
          IsTopLevel : 1);
  } Bits;

public:
  Decl() = delete;
  Decl(const Decl &) = delete;
  Decl(Decl &&) = delete;
  Decl &operator=(const Decl &) = delete;
  Decl &operator=(Decl &&) = delete;

public:
  Decl(DeclKind kind) : kind(kind) {}
};

class ImportDecl : public Decl {
public:
  ImportDecl() : Decl(DeclKind::Import) {}
};

} // namespace syntax

} // end namespace clang

#endif
