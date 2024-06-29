//=== SrcMgrAdapter.cpp - SourceMgr to SrcMgr Adapter -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the adapter that maps diagnostics from llvm::SourceMgr
// to Clang's SrcMgr.
//
//===----------------------------------------------------------------------===//

#include "clang/Basic/SrcMgrAdapter.h"
#include "clang/Basic/DiagnosticEngine.h"

using namespace clang;

void SrcMgrAdapter::handleDiag(const llvm::SMDiagnostic &Diag, void *Context) {
  static_cast<SrcMgrAdapter *>(Context)->handleDiag(Diag);
}

SrcMgrAdapter::SrcMgrAdapter(SrcMgr &SM, DiagnosticEngine &Diagnostics,
                             unsigned ErrorDiagID, unsigned WarningDiagID,
                             unsigned NoteDiagID,
                             OptionalFileEntryRef DefaultFile)
    : SM(SM), Diagnostics(Diagnostics), ErrorDiagID(ErrorDiagID),
      WarningDiagID(WarningDiagID), NoteDiagID(NoteDiagID),
      DefaultFile(DefaultFile) {}

SrcMgrAdapter::~SrcMgrAdapter() {}

SrcLoc SrcMgrAdapter::mapLocation(const llvm::SourceMgr &LLVMSrcMgr,
                                  llvm::SMLoc Loc) {
  // Map invalid locations.
  if (!Loc.isValid())
    return SrcLoc();

  // Find the buffer containing the location.
  unsigned BufferID = LLVMSrcMgr.FindBufferContainingLoc(Loc);
  if (!BufferID)
    return SrcLoc();

  // If we haven't seen this buffer before, copy it over.
  auto Buffer = LLVMSrcMgr.getMemoryBuffer(BufferID);
  auto KnownBuffer = FileIDMapping.find(std::make_pair(&LLVMSrcMgr, BufferID));
  if (KnownBuffer == FileIDMapping.end()) {
    FileID FileID;
    if (DefaultFile) {
      // Map to the default file.
      FileID = SM.getOrCreateFileID(*DefaultFile, sm::C_User);

      // Only do this once.
      DefaultFile = std::nullopt;
    } else {
      // Make a copy of the memory buffer.
      StringRef bufferName = Buffer->getBufferIdentifier();
      auto bufferCopy = std::unique_ptr<llvm::MemoryBuffer>(
          llvm::MemoryBuffer::getMemBufferCopy(Buffer->getBuffer(),
                                               bufferName));

      // Add this memory buffer to the Clang source manager.
      FileID = SM.createFileID(std::move(bufferCopy));
    }

    // Save the mapping.
    KnownBuffer = FileIDMapping
                      .insert(std::make_pair(
                          std::make_pair(&LLVMSrcMgr, BufferID), FileID))
                      .first;
  }

  // Translate the offset into the file.
  unsigned Offset = Loc.getPointer() - Buffer->getBufferStart();
  return SM.getLocForStartOfFile(KnownBuffer->second).getLocWithOffset(Offset);
}

SrcRange SrcMgrAdapter::mapRange(const llvm::SourceMgr &LLVMSrcMgr,
                                 llvm::SMRange Range) {
  if (!Range.isValid())
    return SrcRange();

  SrcLoc Start = mapLocation(LLVMSrcMgr, Range.Start);
  SrcLoc End = mapLocation(LLVMSrcMgr, Range.End);
  return SrcRange(Start, End);
}

void SrcMgrAdapter::handleDiag(const llvm::SMDiagnostic &Diag) {
  // Map the location.
  SrcLoc Loc;
  if (auto *LLVMSrcMgr = Diag.getSourceMgr())
    Loc = mapLocation(*LLVMSrcMgr, Diag.getLoc());

  // Extract the message.
  StringRef Message = Diag.getMessage();

  // Map the diagnostic kind.
  unsigned DiagID;
  switch (Diag.getKind()) {
  case llvm::SourceMgr::DK_Error:
    DiagID = ErrorDiagID;
    break;

  case llvm::SourceMgr::DK_Warning:
    DiagID = WarningDiagID;
    break;

  case llvm::SourceMgr::DK_Remark:
    llvm_unreachable("remarks not implemented");

  case llvm::SourceMgr::DK_Note:
    DiagID = NoteDiagID;
    break;
  }
  // TODO: Will not work
  //  Report the diagnostic.
  //  InflightDiagnostic Builder = Diagnostics.Report(Loc, DiagID) << Message;

  // if (auto *LLVMSrcMgr = Diag.getSourceMgr()) {
  //   // Translate ranges.
  //   SrcLoc StartOfLine = Loc.getLocWithOffset(-Diag.getColumnNo());
  //   for (auto Range : Diag.getRanges()) {
  //     Builder << SrcRange(StartOfLine.getLocWithOffset(Range.first),
  //                            StartOfLine.getLocWithOffset(Range.second));
  //   }

  //   // Translate Fix-Its.
  //   for (const llvm::SMFixIt &FixIt : Diag.getFixIts()) {
  //     CharSrcRange Range(mapRange(*LLVMSrcMgr, FixIt.getRange()), false);
  //     Builder << FixItHint::CreateReplacement(Range, FixIt.getText());
  //   }
  // }
}
