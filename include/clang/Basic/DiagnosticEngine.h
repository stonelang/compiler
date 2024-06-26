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
class DiagnosticEngine;
class QualType;
enum class TokenKind;
class DeclName;
class DiagnosticEngine;
class InFlightDiagnostic;

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

/// Describes the level of the diagnostic
enum class DiagnosticLevel : uint8_t {
  None = 0,
  Ignore,
  Note, ///< Lowest
  Remark,
  Error,
  Fatal, ///< Hightest
};

/// Describes the kind of diagnostic.
enum class DiagnosticKind : uint8_t {
  Error,
  Warning,
  Remark,
  Note,
  Fatal,
};

class FixIt final {
public:
  /// Code that should be replaced to correct the error. Empty for an
  /// insertion hint.
  CharSrcRange RemoveRange;

  /// Code in the specific range that should be inserted in the insertion
  /// location.
  CharSrcRange InsertFromRange;

  /// The actual code to insert at the insertion location, as a
  /// string.
  std::string CodeToInsert;

  bool BeforePreviousInsertions = false;

  /// Empty code modification hint, indicating that no code
  /// modification is known.
  FixIt() = default;

  bool isNull() const { return !RemoveRange.isValid(); }

  /// Create a code modification hint that inserts the given
  /// code string at a specific location.
  static FixIt CreateInsertion(SrcLoc InsertionLoc, StringRef Code,
                               bool BeforePreviousInsertions = false) {
    FixIt Hint;
    Hint.RemoveRange = CharSrcRange::getCharRange(InsertionLoc, InsertionLoc);
    Hint.CodeToInsert = std::string(Code);
    Hint.BeforePreviousInsertions = BeforePreviousInsertions;
    return Hint;
  }

  /// Create a code modification hint that inserts the given
  /// code from \p FromRange at a specific location.
  static FixIt CreateInsertionFromRange(SrcLoc InsertionLoc,
                                        CharSrcRange FromRange,
                                        bool BeforePreviousInsertions = false) {
    FixIt Hint;
    Hint.RemoveRange = CharSrcRange::getCharRange(InsertionLoc, InsertionLoc);
    Hint.InsertFromRange = FromRange;
    Hint.BeforePreviousInsertions = BeforePreviousInsertions;
    return Hint;
  }

  /// Create a code modification hint that removes the given
  /// source range.
  static FixIt CreateRemoval(CharSrcRange RemoveRange) {
    FixIt Hint;
    Hint.RemoveRange = RemoveRange;
    return Hint;
  }
  static FixIt CreateRemoval(SourceRange RemoveRange) {
    return CreateRemoval(CharSrcRange::getTokenRange(RemoveRange));
  }

  /// Create a code modification hint that replaces the given
  /// source range with the given code string.
  static FixIt CreateReplacement(CharSrcRange RemoveRange, StringRef Code) {
    FixIt Hint;
    Hint.RemoveRange = RemoveRange;
    Hint.CodeToInsert = std::string(Code);
    return Hint;
  }

  static FixIt CreateReplacement(SourceRange RemoveRange, StringRef Code) {
    return CreateReplacement(CharSrcRange::getTokenRange(RemoveRange), Code);
  }
};

// class DiagnosticStorageAllocator {};

class StreamingDiagnostic {

protected:
  DiagnosticEngine &diagEngine;

  /// Status variable indicating if this diagnostic is still active.
  ///
  // NOTE: This field is redundant with DiagObj (IsActive iff (DiagObj ==0)),
  // but LLVM is not currently smart enough to eliminate the null check that
  // Emit() would end up with if we used that as our status variable.
  mutable bool IsActive = false;

  /// Flag indicating that this diagnostic is being emitted via a
  /// call to ForceEmit.
  mutable bool IsForceEmit = false;

protected:
  StreamingDiagnostic(DiagnosticEngine &diagEngine) : diagEngine(diagEngine) {}

public:
};

class InFlightDiagnostic final : public StreamingDiagnostic {
public:
  InFlightDiagnostic(DiagnosticEngine &diagEngine)
      : StreamingDiagnostic(diagEngine) {}
};

// class InflightDiagnosticInfo final {

//   const DiagnosticEngine &diagEngine;

// public:
//   InflightDiagnosticInfo(const DiagnosticEngine &diagEngine)
//       : diagEngine(diagEngine) {}

// public:
//   void FormatDiagnostic(llvm::SmallVectorImpl<char> &outString) const;

//   /// Format the given format-string into the output buffer using the
//   /// arguments stored in this diagnostic.
//   void FormatDiagnostic(const char *diagStartString, const char
//   *diagEndString,
//                         llvm::SmallVectorImpl<char> &outString) const;
// };

// class CurrentDiagnostic {

// public:
// };

// class DiagnosticList final {

// public:
//   /// Given a diagnostic ID, return a description of the issue.
//   llvm::StringRef GetDescription(DiagID diagID) const;
//   /// Used to report a diagnostic that is finally fully formed.
//   ///
//   /// \returns \c true if the diagnostic was emitted, \c false if it was
//   /// suppressed.
//   bool ProcessDiagnostic(DiagnosticEngine &diag) const;

//   /// Used to emit a diagnostic that is finally fully formed,
//   /// ignoring suppression.
//   void EmitDiagnostic(DiagnosticEngine &diag, DiagnosticLevel level) const;
// };

// class DiagnosticState {};

// class DiagnosticConsumerInfo {
// public:

// };

class DiagnosticInfo {
protected:
  friend DiagnosticEngine;
  friend InFlightDiagnostic;

  DiagID diagID;
  SrcLoc diagLoc;
  DiagnosticKind kind;
  llvm::StringRef formatString;
  llvm::SmallVector<DiagnosticArgument, 3> args;
  llvm::SmallVector<FixIt, 2> fixIts;

protected:
  DiagnosticInfo(DiagID diagID) : diagID(diagID) {}

public:
  DiagID GetDiagID() const { return diagID; }
  llvm::ArrayRef<DiagnosticArgument> GetArgs() const { return args; }
  llvm::ArrayRef<FixIt> GetFixIts() const { return fixIts; }
  SrcLoc GetDiagLoc() { return diagLoc; }
  DiagnosticKind GetDiagKind() { return kind; }
  llvm::StringRef GetFormatString() { return formatString; }
};

class Diagnostic final : public DiagnosticInfo {
public:
  Diagnostic(DiagID diagID) : DiagnosticInfo(diagID) {}

public:
  // Avoid copying the fix-it text more than necessary.
  void AddFixIt(FixIt &&fixIt) { fixIts.push_back(std::move(fixIt)); }
  void SetDiagLoc(SrcLoc loc) { diagLoc = loc; }
};

class DiagnosticEngine final {
  /// The ID of the current diagnostic that is in flight.
  std::optional<Diagnostic> activeDiagnostic;

public:
  DiagnosticEngine();

public:
  /// Determine whethere there is already a diagnostic in flight.
  bool HasActiveDiagnostic() const { return activeDiagnostic.has_value(); }
  Diagnostic &GetActiveDiagnostic() { return *activeDiagnostic; }
  /// Flush the active diagnostic.
  void FlushActiveDiagnostic();

  /// Used to report a diagnostic that is finally fully formed.
  ///
  /// \returns true if the diagnostic was emitted, false if it was suppressed.
  bool ProcessDiagnostic() {
    // return Diags->ProcessDiag(*this);
  }

  /// Emit the current diagnostic and clear the diagnostic state.
  ///
  /// \param Force Emit the diagnostic regardless of suppression settings.
  bool EmitActiveDiagnostic(bool force = false);

  /// Emit the current diagnostic and clear the diagnostic state.
  ///
  /// \param Force Emit the diagnostic regardless of suppression settings.
  bool EmitDiagnostic(const Diagnostic &diagnostic, bool force = false);

public:
  /// Issue the message to the client.
  ///
  /// This actually returns an instance of InflightDiagnostic which emits the
  /// diagnostics (through @c ProcessDiag) when it is destroyed.
  ///
  /// \param DiagID A member of the @c diag::kind enum.
  /// \param Loc Represents the source location associated with the diagnostic,
  /// which can be an invalid location if no position information is available.
  InFlightDiagnostic Diagnose(SrcLoc Loc, DiagID diagID);
  InFlightDiagnostic Diagnose(DiagID diagID);
  InFlightDiagnostic Diagnose(SrcLoc diagLoc, const Diagnostic &diagnostic);

public:
  void SetSrcMgr(SrcMgr *SM) {}
};
} // namespace clang

#endif