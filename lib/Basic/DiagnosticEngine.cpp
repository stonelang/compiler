#include "clang/Basic/DiagnosticEngine.h"

using namespace clang;


InflightDiagnostic DiagnosticEngine::Report(SrcLoc Loc, unsigned DiagID) {
	return InflightDiagnostic();


}

  	InflightDiagnostic DiagnosticEngine::Report(unsigned DiagID) {

  		return InflightDiagnostic();
  	}
