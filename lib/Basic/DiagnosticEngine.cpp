#include "clang/Basic/DiagnosticEngine.h"

using namespace clang;

DiagnosticEngine::DiagnosticEngine() {}

InFlightDiagnostic DiagnosticEngine::Diagnose(DiagID diagID, SrcLoc diagLoc) {
  return Diagnose(Diagnostic(diagID), diagLoc);
}

InFlightDiagnostic DiagnosticEngine::Diagnose(const Diagnostic &diagnostic,
                                              SrcLoc diagLoc) {
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
