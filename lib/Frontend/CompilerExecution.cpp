#include "clang/Frontend/CompilerExecution.h"

using namespace clang;

CompilerExecution::CompilerExecution(CompilerInstance &instance)
    : instance(instance) {}


CompilerExecution::~CompilerExecution() {
}

bool CompilerExecution::Initialize() {
  instance.LoadRequestedPlugins();
  if (instance.getDiagnostics().hasErrorOccurred()) {
    return false;
  }
  return true;
}

bool CompilerExecution::ExecuteCodeAnalysis() { return true; }