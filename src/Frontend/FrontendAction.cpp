//
// Created by Sirui Mu on 2019/12/23.
//

#include "Frontend/FrontendAction.h"
#include "Frontend/CompilerInstance.h"
#include "Frontend/Diagnostics.h"

#include "BuiltinFrontendActions.h"

namespace jvc {

std::unique_ptr<FrontendAction> FrontendAction::CreateAction(FrontendActionKind kind, DiagnosticsEngine& diag) {
  switch (kind) {
    case FrontendActionKind::LexOnly:
      return std::make_unique<LexOnlyFrontendAction>();
    default: {
      auto diagMsg = DiagnosticsMessage::CreateLiteral(DiagnosticsLevel::Fatal, "Unsupported action type.");
      diag.Emit(*diagMsg);
      return std::unique_ptr<FrontendAction> { };
    }
  }
}

} // namespace jvc
