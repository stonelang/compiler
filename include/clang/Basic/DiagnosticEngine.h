#ifndef LLVM_CLANG_BASIC_DIAGNOSTIC_H
#define LLVM_CLANG_BASIC_DIAGNOSTIC_H

#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/SrcLoc.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Compiler.h"

#include <cassert>
#include <cstdint>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace llvm {
class Error;
class raw_ostream;
} // namespace llvm

namespace clang {
class Decl;
class Identifier;
class QualType;
enum class TokenKind;
class DeclName;

// Enumeration describing all of possible diagnostics.
/// Each of the diagnostics described in Diagnostics.def has an entry in
/// this enumeration type that uniquely identifies it.
enum class DiagID : unsigned;

/// Describes a diagnostic along with its argument types.
///
/// The diagnostics header introduces instances of this type for each
/// diagnostic, which provide both the set of argument types (used to
/// check/convert the arguments at each call site) and the diagnostic ID
/// (for other information about the diagnostic).
template <typename... ArgTypes> struct Diag {
  /// The diagnostic ID corresponding to this diagnostic.
  DiagID ID;
};

namespace detail {
/// Describes how to pass a diagnostic argument of the given type.
///
/// By default, diagnostic arguments are passed by value, because they
/// tend to be small. Larger diagnostic arguments
/// need to specialize this class template to pass by reference.
template <typename T> struct PassArgument {
  typedef T type;
};
} // namespace detail

template <class... ArgTypes>
using DiagArgTuple =
    std::tuple<typename detail::PassArgument<ArgTypes>::type...>;

enum class FixItID : unsigned;

/// Represents a fix-it defined  with a format string and optional
/// DiagnosticArguments. The template parameters allow the
/// fixIt... methods on InFlightDiagnostic to infer their own
/// template params.
template <typename... ArgTypes> struct StructuredFixIt {
  FixItID ID;
};

enum class DiagnosticArgumentKind {
  Bool,
  String,
  Integer,
  Unsigned,
  Identifier,
  Decl,
  QualType,
  TokenKind
};

class DiagnosticArgument final {
  DiagnosticArgumentKind kind;
  union {
    bool BoolVal;
    int IntegerVal;
    unsigned UnsignedVal;
    llvm::StringRef StringVal;
    const Identifier *IdentifierVal;
    const Decl *DeclVal;
    const QualType *QualTypeVal;
    const DeclName *DeclNameVal;
    TokenKind TokenKindVal;
  };

public:
  DiagnosticArgument(bool val)
      : kind(DiagnosticArgumentKind::Bool), BoolVal(val) {}

  DiagnosticArgument(StringRef val)
      : kind(DiagnosticArgumentKind::String), StringVal(val) {}

  DiagnosticArgument(int val)
      : kind(DiagnosticArgumentKind::Integer), IntegerVal(val) {}

  DiagnosticArgument(unsigned val)
      : kind(DiagnosticArgumentKind::Unsigned), UnsignedVal(val) {}

  DiagnosticArgument(const Identifier *val)
      : kind(DiagnosticArgumentKind::Identifier), IdentifierVal(val) {}

  DiagnosticArgument(const Decl *val)
      : kind(DiagnosticArgumentKind::Decl), DeclVal(val) {}

  DiagnosticArgument(const QualType *val)
      : kind(DiagnosticArgumentKind::QualType), QualTypeVal(val) {}

  DiagnosticArgument(TokenKind val)
      : kind(DiagnosticArgumentKind::TokenKind), TokenKindVal(val) {}

  /// Initializes a diagnostic argument using the underlying type of the
  /// given enum.
  template <
      typename EnumType,
      typename std::enable_if<std::is_enum<EnumType>::value>::type * = nullptr>
  DiagnosticArgument(EnumType value)
      : DiagnosticArgument(
            static_cast<typename std::underlying_type<EnumType>::type>(value)) {
  }

public:
  DiagnosticArgumentKind GetKind() const { return kind; }
};

class StreamingDiagnostic {
public:
};

class InflightDiagnostic final : public StreamingDiagnostic {
public:
};

enum class DiagnosticLevel {
  None = 0,
  Ignored = 1, ///< Do not present this diagnostic, ignore it.
  Remark = 2,  ///< Present this diagnostic as a remark.
  Warning = 3, ///< Present this diagnostic as a warning.
  Error = 4,   ///< Present this diagnostic as an error.
  Fatal = 5    ///< Present this diagnostic as a fatal error.
};

class DiagnosticEngine final {
  /// The ID of the current diagnostic that is in flight.
  ///
  /// This is set to std::numeric_limits<unsigned>::max() when there is no
  /// diagnostic in flight.
  std::optional<DiagID> CurDiagID;

public:
  /// Determine whethere there is already a diagnostic in flight.
  bool IsInflightDiagnostic() const { return !CurDiagID; }

  void SetSrcMgr(SrcMgr *SM) {}
  /// Issue the message to the client.
  ///
  /// This actually returns an instance of InflightDiagnostic which emits the
  /// diagnostics (through @c ProcessDiag) when it is destroyed.
  ///
  /// \param DiagID A member of the @c diag::kind enum.
  /// \param Loc Represents the source location associated with the diagnostic,
  /// which can be an invalid location if no position information is available.
  InflightDiagnostic Diagnose(SrcLoc Loc, DiagID diagID);
  InflightDiagnostic Diagnose(DiagID diagID);
};
} // namespace clang

#endif