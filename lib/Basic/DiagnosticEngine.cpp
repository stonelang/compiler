#include "clang/Basic/DiagnosticEngine.h"

using namespace clang;

InFlightDiagnostic DiagnosticEngine::Diagnose(SrcLoc Loc, DiagID diagID) {
  return InFlightDiagnostic();
}

InFlightDiagnostic DiagnosticEngine::Diagnose(DiagID diagID) {

  return InFlightDiagnostic();
}
