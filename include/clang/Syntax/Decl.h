#ifndef LLVM_CLANG_SYNTAX_DECL_H
#define LLVM_CLANG_SYNTAX_DECL_H

#include "clang/AST/DeclarationName.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Core/InlineBitfield.h"
#include "clang/Syntax/ASTAllocation.h"
#include "clang/Syntax/DeclContext.h"
#include "clang/Syntax/Type.h"
#include "clang/Syntax/TypeAlignment.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/VersionTuple.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

#include <memory>

namespace clang {
namespace syn {

class Type;
class ModuleDecl;
class QualType;

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

// Captures the result of checking the availability of a
/// declaration.
enum class DeclAvailability : uint8_t {
  Available = 0,
  NotYetIntroduced,
  Deprecated,
  Unavailable
};

class alignas(1 << DeclAlignInBits) Decl : public ASTAllocation<Decl> {
  DeclKind kind;
  SourceLocation loc;

public:
  /// The kind of ownership a declaration has, for visibility purposes.
  /// This enumeration is designed such that higher values represent higher
  /// levels of name hiding.
  enum class ModuleOwnershipKind : uint8_t {
    /// This declaration is not owned by a module.
    Unowned = 0,

    /// This declaration has an owning module, but is globally visible
    /// (typically because its owning module is visible and we know that
    /// modules cannot later become hidden in this compilation).
    /// After serialization and deserialization, this will be converted
    /// to VisibleWhenImported.
    Visible,

    /// This declaration has an owning module, and is visible when that
    /// module is imported.
    VisibleWhenImported,

    /// This declaration has an owning module, and is visible to lookups
    /// that occurs within that module. And it is reachable in other module
    /// when the owning module is transitively imported.
    ReachableWhenImported,

    /// This declaration has an owning module, but is only visible to
    /// lookups that occur within that module.
    /// The discarded declarations in global module fragment belongs
    /// to this group too.
    ModulePrivate
  };

protected:
  /// The next declaration within the same lexical
  /// DeclContext. These pointers form the linked list that is
  /// traversed via DeclContext's decls_begin()/decls_end().
  ///
  /// The extra three bits are used for the ModuleOwnershipKind.
  llvm::PointerIntPair<Decl *, 3, ModuleOwnershipKind> NextInContextAndBits;

private:
  friend class DeclContext;

  // TODO: Seems so odd to care about anything lexical here.
  struct MultipleDeclContext final {
    DeclContext *SemanticDeclContext;
    DeclContext *LexicalDeclContext;
  };

  /// JointDeclContext - Holds either a DeclContext* or a MultipleDeclContext*.
  /// For declarations that don't contain C++ scope specifiers, it contains
  /// the DeclContext where the Decl was declared.
  /// For declarations with C++ scope specifiers, it contains a MultipleDC*
  /// with the context where it semantically belongs (SemanticDC) and the
  /// context where it was lexically declared (LexicalDC).
  /// e.g.:
  ///
  ///   space A {
  ///      void f(); // SemanticDeclContext == LexicalDC == 'namespace A'
  ///   }
  ///   void A::f(); // SemanticDeclContext == namespace 'A'
  ///                // LexicalDeclContext == global namespace
  llvm::PointerUnion<DeclContext *, MultipleDeclContext *> JointDeclContext;

  bool IsInSemanticDeclContext() const {
    return JointDeclContext.is<DeclContext *>();
  }
  bool IsOutOfSemanticDeclContext() const {
    return JointDeclContext.is<MultipleDeclContext *>();
  }

  MultipleDeclContext *GetMultipleDeclContext() const {
    return JointDeclContext.get<MultipleDeclContext *>();
  }
  DeclContext *GetSemanticDeclContext() const {
    return JointDeclContext.get<DeclContext *>();
  }

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
          Referenced : 1,

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
  Decl(DeclKind kind, DeclContext *dc, SourceLocation loc) : kind(kind) {}

public:
  DeclKind GetKind() { return kind; }
};

class NamedDecl : public Decl {
  /// The name of this declaration, which is typically a normal
  /// identifier but may also be a special kind of name (C++
  /// constructor, etc.)
  DeclarationName name;

protected:
  NamedDecl(DeclKind kind, DeclContext *dc, SourceLocation loc,
            DeclarationName name)
      : Decl(kind, dc, loc), name(name) {}
};

class ValueDecl : public NamedDecl {
  QualType declType;
};

class DeclaratorDecl : public ValueDecl {
public:
};

class FunctionDecl : public DeclaratorDecl {

public:
};

class FunDecl : public FunctionDecl {

public:
};

class ConstructDecl : public FunctionDecl {

public:
};

class DesctructorDecl : public FunctionDecl {

public:
};

class TypeDecl : public NamedDecl {
  friend class ASTContext;

  /// This indicates the Type object that represents
  /// this TypeDecl.  It is a cache maintained by
  /// ASTContext::tetTypedefType, ASTContext::getTagDeclType, and
  /// ASTContext::getTemplateTypeParmType, and TemplateTypeParmDecl.
  mutable const Type *typeForDecl = nullptr;

  /// The start of the source range for this declaration.
  SourceLocation startLoc;

protected:
  TypeDecl(DeclKind kind, DeclContext *dc, SourceLocation loc,
           const IdentifierInfo *identifier,
           SourceLocation startLoc = SourceLocation())
      : NamedDecl(kind, dc, loc, identifier), startLoc(startLoc) {}
};

// class ImportDecl : public Decl {
/// The resolved module.
// ModuleDecl *Mod = nullptr;
// public:
//   ImportDecl(DeclContext *dc, SourceLocation loc,
//             DeclarationName name) : Decl(DeclKind::Import, dc, ) {}
// };

// TODO: Redeclarable<NominalType>
class NominalTypeDecl : public TypeDecl {
public:
};

class EnumDecl : public NominalTypeDecl {
public:
};

class StructDecl : public NominalTypeDecl {
public:
};

class ClassDecl : public NominalTypeDecl {
public:
};

class InterfaceDecl : public NominalTypeDecl {
public:
};

// public Redeclarable<AliasNameDecl>
class AliasNameDecl : public TypeDecl {
public:
};
class AliasDecl : public AliasNameDecl {
public:
};

} // namespace syn

} // end namespace clang

#endif
