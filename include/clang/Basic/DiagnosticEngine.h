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


	class StreamingDiagnostic {


	};
	class InflightDiagnostic : public StreamingDiagnostic {


	};

	
	class DiagnosticEngine final {
		/// The ID of the current diagnostic that is in flight.
  ///
  /// This is set to std::numeric_limits<unsigned>::max() when there is no
  /// diagnostic in flight.
  	unsigned CurDiagID;


	public:
		 /// Clear out the current diagnostic.
  	void Clear() { CurDiagID = std::numeric_limits<unsigned>::max(); }
  	  /// Determine whethere there is already a diagnostic in flight.
  bool IsInflightDiagnostic() const {
    return CurDiagID != std::numeric_limits<unsigned>::max();
  }

  void SetSrcMgr(SrcMgr* SM){
		
  }
  /// Issue the message to the client.
  ///
  /// This actually returns an instance of InflightDiagnostic which emits the
  /// diagnostics (through @c ProcessDiag) when it is destroyed.
  ///
  /// \param DiagID A member of the @c diag::kind enum.
  /// \param Loc Represents the source location associated with the diagnostic,
  /// which can be an invalid location if no position information is available.
  	InflightDiagnostic Report(SrcLoc Loc, unsigned DiagID);
  	InflightDiagnostic Report(unsigned DiagID);

	};
}

#endif 