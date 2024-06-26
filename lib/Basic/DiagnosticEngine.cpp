#include "clang/Basic/DiagnosticEngine.h"

using namespace clang;

InflightDiagnostic DiagnosticEngine::Diagnose(SrcLoc Loc, DiagID diagID) {
  return InflightDiagnostic();
}

InflightDiagnostic DiagnosticEngine::Diagnose(DiagID diagID) {

  return InflightDiagnostic();
}
