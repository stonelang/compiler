#ifndef LLVM_CLANG_BASIC_DIAGNOSTIC_H
#define LLVM_CLANG_BASIC_DIAGNOSTIC_H

#include "clang/Basic/DiagnosticAllocation.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/Memory.h"
#include "clang/Basic/SrcLoc.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Casting.h"
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
class DiagnosticBuilder;
class SrcMgr;
class DiagnosticConsumer;

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
/// fixIt... methods on DiagnosticBuilder to infer their own
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

constexpr size_t DiagnosticArgumentInBits = 3;
class alignas(1 << DiagnosticArgumentInBits) DiagnosticArgument final
    : public DiagnosticAllocation<DiagnosticArgument> {
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
  static FixIt CreateRemoval(SrcRange RemoveRange) {
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

  static FixIt CreateReplacement(SrcRange RemoveRange, StringRef Code) {
    return CreateReplacement(CharSrcRange::getTokenRange(RemoveRange), Code);
  }
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
  friend DiagnosticBuilder;

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

public:
  /// Format this diagnostic into a string, substituting the
  /// formal arguments into the %0 slots.
  ///
  /// The result is appended onto the \p OutStr array.
  void FormatDiagnostic(llvm::SmallVectorImpl<char> &outString) const;

  /// Format the given format-string into the output buffer using the
  /// arguments stored in this diagnostic.
  void FormatDiagnostic(const char *diagStart, const char *diagEnd,
                        llvm::SmallVectorImpl<char> &outString) const;
};

class Diagnostic final : public DiagnosticInfo {
public:
  Diagnostic(DiagID diagID) : DiagnosticInfo(diagID) {}

public:
  // Avoid copying the fix-it text more than necessary.
  void AddFixIt(FixIt &&fixIt) { fixIts.push_back(std::move(fixIt)); }
  void AddArg(DiagnosticArgument &&arg) { args.push_back(std::move(arg)); }

  // void AddDiagnosticArgument(const DiagnosticArgument *arg) {
  //   args.push_back(arg);
  // }
  void SetDiagLoc(SrcLoc loc) { diagLoc = loc; }
};

class DiagnosticEngine final {
  SrcMgr *srcMgr = nullptr;

  /// The current diagnostic
  std::optional<Diagnostic> activeDiagnostic;

  /// The diagnostic consumer(s) that will be responsible for actually
  /// emitting diagnostics.
  llvm::SmallVector<DiagnosticConsumer *, 2> consumers;

  mutable llvm::BumpPtrAllocator bumpAllocator;

public:
  DiagnosticEngine();
  DiagnosticEngine(const DiagnosticEngine &) = delete;
  DiagnosticEngine &operator=(const DiagnosticEngine &) = delete;

public:
  void *AllocateMemory(size_t bytes, unsigned alignment = 8) const {
    return bumpAllocator.Allocate(bytes, alignment);
  }

  void Deallocate(void *Ptr) const {}

  llvm::BumpPtrAllocator &GetAllocator() const { return bumpAllocator; }
  size_t GetTotalMemUsed() const { return GetAllocator().getTotalMemory(); }

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
  DiagnosticBuilder Diagnose(DiagID diagID, SrcLoc diagLoc = SrcLoc());
  DiagnosticBuilder Diagnose(const Diagnostic &diagnostic,
                             SrcLoc diagLoc = SrcLoc());

public:
  void SetSrcMgr(SrcMgr *sm) { srcMgr = sm; }
  SrcMgr *GetSetSrcMgr() {
    assert(HasSrcMgr() && "Does not have a SrcMgr");
    return srcMgr;
  }
  bool HasSrcMgr() { return srcMgr != nullptr; }

public:
  template <typename T> T *Allocate() const {
    T *res = (T *)AllocateMemory(sizeof(T), alignof(T));
    new (res) T();
    return res;
  }

  template <typename T>
  MutableArrayRef<T> AllocateUninitialized(unsigned NumElts) const {
    T *Data = (T *)AllocateMemory(sizeof(T) * NumElts, alignof(T));
    return {Data, NumElts};
  }

  template <typename T> MutableArrayRef<T> Allocate(unsigned numElts) const {
    T *res = (T *)AllocateMemory(sizeof(T) * numElts, alignof(T));
    for (unsigned i = 0; i != numElts; ++i)
      new (res + i) T();
    return {res, numElts};
  }

  /// Allocate a copy of the specified object.
  template <typename T>
  typename std::remove_reference<T>::type *AllocateObjectCopy(T &&t) const {
    // This function cannot be named AllocateCopy because it would always win
    // overload resolution over the AllocateCopy(ArrayRef<T>).
    using TNoRef = typename std::remove_reference<T>::type;
    TNoRef *res = (TNoRef *)AllocateMemory(sizeof(TNoRef), alignof(TNoRef));
    new (res) TNoRef(std::forward<T>(t));
    return res;
  }

  template <typename T, typename It> T *AllocateCopy(It start, It end) const {
    T *res = (T *)AllocateMemory(sizeof(T) * (end - start), alignof(T));
    for (unsigned i = 0; start != end; ++start, ++i)
      new (res + i) T(*start);
    return res;
  }

  template <typename T, size_t N>
  MutableArrayRef<T> AllocateCopy(T (&array)[N]) const {
    return MutableArrayRef<T>(AllocateCopy<T>(array, array + N), N);
  }

  template <typename T>
  MutableArrayRef<T> AllocateCopy(ArrayRef<T> array) const {
    return MutableArrayRef<T>(AllocateCopy<T>(array.begin(), array.end()),
                              array.size());
  }

  template <typename T>
  ArrayRef<T> AllocateCopy(const SmallVectorImpl<T> &vec) const {
    return AllocateCopy(ArrayRef<T>(vec));
  }

  template <typename T>
  MutableArrayRef<T> AllocateCopy(SmallVectorImpl<T> &vec) const {
    return AllocateCopy(MutableArrayRef<T>(vec));
  }

  StringRef AllocateCopy(StringRef Str) const {
    ArrayRef<char> Result =
        AllocateCopy(llvm::ArrayRef(Str.data(), Str.size()));
    return StringRef(Result.data(), Result.size());
  }

  template <typename T, typename Vector, typename Set>
  MutableArrayRef<T>
  AllocateCopy(llvm::SetVector<T, Vector, Set> setVector) const {
    return MutableArrayRef<T>(
        AllocateCopy<T>(setVector.begin(), setVector.end()), setVector.size());
  }
};

class DiagnosticBuilder final {

  DiagnosticEngine &diagEngine;

  /// Status variable indicating if this diagnostic is still active.
  ///
  // NOTE: This field is redundant with DiagObj (IsActive iff (DiagObj ==0)),
  // but LLVM is not currently smart enough to eliminate the null check that
  // Emit() would end up with if we used that as our status variable.
  mutable bool isActive = false;

  /// Flag indicating that this diagnostic is being emitted via a
  /// call to ForceEmit.
  mutable bool forceFlush = false;

  /// Force the diagnostic builder to emit the diagnostic now.
  ///
  /// Once this function has been called, the DiagnosticBuilder object
  /// should not be used again before it is destroyed.
  ///
  /// \returns true if a diagnostic was emitted, false if the
  /// diagnostic was suppressed.
  bool Flush() {
    // If this diagnostic is inactive, then its soul was stolen by the copy ctor
    // (or by a subclass, as in DiagnosticBuilder).
    if (!isActive) {
      return false;
    }

    // Process the diagnostic.
    return diagEngine.EmitActiveDiagnostic(forceFlush);
  }

  Diagnostic &GetActiveDiagnostic() { return diagEngine.GetActiveDiagnostic(); }

public:
  DiagnosticBuilder(DiagnosticEngine &diagEngine)
      : diagEngine(diagEngine), isActive(true) {}

  DiagnosticBuilder &operator=(const DiagnosticBuilder &) = delete;

  /// Emits the diagnostic.
  ~DiagnosticBuilder() {
    if (isActive) {
      Flush();
    }
  }

public:
  DiagnosticEngine &GetDiags() { return diagEngine; }

  DiagnosticBuilder AddArg(bool val) {
    GetActiveDiagnostic().AddArg(DiagnosticArgument(val));
    return *this;
  }
  DiagnosticBuilder AddArg(int val) {
    GetActiveDiagnostic().AddArg(DiagnosticArgument(val));
    return *this;
  }
  DiagnosticBuilder AddArg(unsigned val) {
    GetActiveDiagnostic().AddArg(DiagnosticArgument(val));
    return *this;
  }
  DiagnosticBuilder AddArg(llvm::StringRef val) {
    GetActiveDiagnostic().AddArg(DiagnosticArgument(val));
    return *this;
  }

  DiagnosticBuilder AddArg(Identifier *val) {
    GetActiveDiagnostic().AddArg(DiagnosticArgument(val));
    return *this;
  }

  DiagnosticBuilder AddArg(Decl *val) {
    GetActiveDiagnostic().AddArg(DiagnosticArgument(val));
    return *this;
  }
  DiagnosticBuilder AddArg(QualType *val) {
    GetActiveDiagnostic().AddArg(DiagnosticArgument(val));
    return *this;
  }
  DiagnosticBuilder AddArg(DeclName *val) {
    GetActiveDiagnostic().AddArg(DiagnosticArgument(val));
    return *this;
  }

  DiagnosticBuilder AddArg(TokenKind val) {
    GetActiveDiagnostic().AddArg(DiagnosticArgument(val));
    return *this;
  }

  DiagnosticBuilder AddInsertionFixIt(SrcLoc insertionLoc, llvm::StringRef code,
                                      bool beforePreviousInsertions = false) {
    GetActiveDiagnostic().AddFixIt(
        FixIt::CreateInsertion(insertionLoc, code, beforePreviousInsertions));
    return *this;
  }

  DiagnosticBuilder
  AddInsertionFromRangeFixit(SrcLoc InsertionLoc, CharSrcRange FromRange,
                             bool BeforePreviousInsertions = false) {
    GetActiveDiagnostic().AddFixIt(FixIt::CreateInsertionFromRange(
        InsertionLoc, FromRange, BeforePreviousInsertions));
    return *this;
  }

  DiagnosticBuilder AddRemovalFixIt(CharSrcRange RemoveRange) {
    GetActiveDiagnostic().AddFixIt(FixIt::CreateRemoval(RemoveRange));
    return *this;
  }
  DiagnosticBuilder AddRemovalFixIt(SrcRange RemoveRange) {
    return AddRemovalFixIt(CharSrcRange::getTokenRange(RemoveRange));
  }

  DiagnosticBuilder AddReplacementFixIt(CharSrcRange RemoveRange,
                                        StringRef Code) {

    GetActiveDiagnostic().AddFixIt(FixIt::CreateReplacement(RemoveRange, Code));
    return *this;
  }

  DiagnosticBuilder AddReplacementFixIt(SrcRange RemoveRange, StringRef Code) {
    return AddReplacementFixIt(CharSrcRange::getTokenRange(RemoveRange), Code);
  }
};

class DiagnosticConsumer {
protected:
  unsigned numWarnings = 0; ///< Number of warnings reported
  unsigned numErrors = 0;   ///< Number of errors reported

public:
  DiagnosticConsumer() = default;
  virtual ~DiagnosticConsumer();

  unsigned GetNumErrors() const { return numWarnings; }
  unsigned GetNumWarnings() const { return numWarnings; }
  virtual void Clear() { numWarnings = numWarnings = 0; }

  /// Callback to inform the diagnostic client that processing of all
  /// source files has ended.
  virtual bool FinishProcessing() { return false; }

  /// Handle this diagnostic, reporting it to the user or
  /// capturing it to a log as needed.
  ///
  /// The default implementation just keeps track of the total number of
  /// warnings and errors.
  virtual void HandleDiagnostic(DiagnosticLevel diagLevel,
                                const Diagnostic &diagnostic);
};
} // namespace clang

#endif