#include "clang/Basic/DiagnosticEngine.h"

using namespace clang;

DiagnosticEngine::DiagnosticEngine() {}

InFlightDiagnostic DiagnosticEngine::Diagnose(SrcLoc Loc, DiagID diagID) {
  return InFlightDiagnostic(*this);
}

InFlightDiagnostic DiagnosticEngine::Diagnose(DiagID diagID) {

  return InFlightDiagnostic(*this);
}

InFlightDiagnostic DiagnosticEngine::Diagnose(SrcLoc diagLoc,
                                              const Diagnostic &diagnostic) {
  assert(!activeDiagnostic && "Already have an active diagnostic!");
  activeDiagnostic = diagnostic;
  activeDiagnostic->SetDiagLoc(diagLoc);
  return InFlightDiagnostic(*this);
}

/// Emit the current diagnostic and clear the diagnostic state.
///
/// \param Force Emit the diagnostic regardless of suppression settings.
bool DiagnosticEngine::EmitActiveDiagnostic(bool force) {

  return EmitDiagnostic(GetActiveDiagnostic(), true);
}

/// Emit the current diagnostic and clear the diagnostic state.
///
/// \param Force Emit the diagnostic regardless of suppression settings.
bool DiagnosticEngine::EmitDiagnostic(const Diagnostic &diagnostic,
                                      bool force) {
  return true; 
}
