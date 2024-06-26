#include "clang/Basic/DiagnosticEngine.h"

using namespace clang;

DiagnosticEngine::DiagnosticEngine() {}

InFlightDiagnostic DiagnosticEngine::Diagnose(SrcLoc Loc, DiagID diagID) {
  return InFlightDiagnostic();
}

InFlightDiagnostic DiagnosticEngine::Diagnose(DiagID diagID) {

  return InFlightDiagnostic();
}
