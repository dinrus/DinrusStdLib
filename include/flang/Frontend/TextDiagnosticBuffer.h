//===- TextDiagnosticBuffer.h - Buffer Text Diagnostics ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This is a concrete diagnostic client. The diagnostics are buffered rather
// than printed. In order to print them, use the FlushDiagnostics method.
// Pretty-printing is not supported.
//
//===----------------------------------------------------------------------===//
//
// Coding style: https://mlir.llvm.org/getting_started/DeveloperGuide/
//
//===----------------------------------------------------------------------===//

#ifndef FORTRAN_FRONTEND_TEXTDIAGNOSTICBUFFER_H
#define FORTRAN_FRONTEND_TEXTDIAGNOSTICBUFFER_H

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/SourceLocation.h"
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace Fortran::frontend {

class TextDiagnosticBuffer : public clang::DiagnosticConsumer {
public:
  using DiagList = std::vector<std::pair<clang::SourceLocation, std::string>>;
  using DiagnosticsLevelAndIndexPairs =
      std::vector<std::pair<clang::DiagnosticsEngine::Level, size_t>>;

private:
  DiagList errors, warnings, remarks, notes;

  /// All diagnostics in the order in which they were generated. That order
  /// likely doesn't correspond to user input order, but at least it keeps
  /// notes in the right places. Each pair is a diagnostic level and an index
  /// into the corresponding DiagList above.
  DiagnosticsLevelAndIndexPairs all;

public:
  void HandleDiagnostic(clang::DiagnosticsEngine::Level diagLevel,
                        const clang::Diagnostic &info) override;

  /// Flush the buffered diagnostics to a given diagnostic engine.
  void flushDiagnostics(clang::DiagnosticsEngine &diags) const;
};

} // namespace Fortran::frontend

#endif // FORTRAN_FRONTEND_TEXTDIAGNOSTICBUFFER_H
