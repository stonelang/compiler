#include "clang/Basic/DiagnosticEngine.h"

using namespace clang;

InflightDiagnostic DiagnosticEngine::Diagnose(SrcLoc Loc, unsigned DiagID) {
  return InflightDiagnostic();
}

InflightDiagnostic DiagnosticEngine::Diagnose(unsigned DiagID) {

  return InflightDiagnostic();
}
