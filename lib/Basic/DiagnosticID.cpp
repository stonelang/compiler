#include "clang/Basic/DiagnosticBasic.h"

using namespace clang;

enum class clang::DiagID : unsigned {
#define DIAG(KIND, ID, Options, Text, Signature) ID,
#include "clang/Basic/DiagnosticEngine.def"
};
static_assert(static_cast<uint32_t>(clang::DiagID::invalid_diagnostic) == 0,
              "0 is not the invalid diagnostic ID");

enum class clang::FixItID : unsigned {
#define DIAG(KIND, ID, Options, Text, Signature)
#define FIXIT(ID, Text, Signature) ID,
#include "clang/Basic/DiagnosticEngine.def"
};

// Define all of the diagnostic objects and initialize them with their
// diagnostic IDs.
namespace clang {
namespace diag {
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  detail::DiagWithArguments<void Signature>::type ID = {DiagID::ID};
#define FIXIT(ID, Text, Signature)                                             \
  detail::StructuredFixItWithArguments<void Signature>::type ID = {FixItID::ID};
#include "clang/Basic/DiagnosticEngine.def"
} // end namespace diag
} // end namespace clang
