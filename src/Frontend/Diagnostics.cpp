//
// Created by Sirui Mu on 2019/12/19.
//

#include "Infrastructure/Stream.h"
#include "Frontend/Diagnostics.h"
#include "Frontend/CompilerInstance.h"

namespace jvc {

namespace {

class LiteralDiagnosticsMessage : public DiagnosticsMessage {
public:
  explicit LiteralDiagnosticsMessage(DiagnosticsLevel level, std::string message)
    : DiagnosticsMessage(level),
      _message(std::move(message))
  { }

  explicit LiteralDiagnosticsMessage(DiagnosticsLevel level, SourceLocation location, std::string message)
    : DiagnosticsMessage(level, location),
      _message(std::move(message))
  { }

  explicit LiteralDiagnosticsMessage(DiagnosticsLevel level, SourceRange range, std::string message)
      : DiagnosticsMessage(level, range),
        _message(std::move(message))
  { }

  [[nodiscard]]
  std::string FormatMessage() const override {
    return _message;
  }

private:
  std::string _message;
};

} // namespace anonymous

std::unique_ptr<DiagnosticsMessage> DiagnosticsMessage::CreateLiteral(DiagnosticsLevel level, std::string message) {
  return std::make_unique<LiteralDiagnosticsMessage>(level, std::move(message));
}

std::unique_ptr<DiagnosticsMessage>
DiagnosticsMessage::CreateLiteral(DiagnosticsLevel level, SourceLocation loc, std::string message) {
  return std::make_unique<LiteralDiagnosticsMessage>(level, loc, std::move(message));
}

std::unique_ptr<DiagnosticsMessage>
DiagnosticsMessage::CreateLiteral(DiagnosticsLevel level, SourceRange range, std::string message) {
  return std::make_unique<LiteralDiagnosticsMessage>(level, range, std::move(message));
}

namespace {

const char* getDiagLevelName(DiagnosticsLevel level) {
  switch (level) {
    case DiagnosticsLevel::Info:
      return "info";
    case DiagnosticsLevel::Warning:
      return "warning";
    case DiagnosticsLevel::Error:
      return "error";
    case DiagnosticsLevel::Fatal:
      return "fatal";
    default:
      return "custom";
  }
}

void dumpSourceLoc(SourceLocation loc) {
  auto& o = errs();
  if (!loc.valid()) {
    o << "<invalid loc>";
    return;
  }

  o << loc.row() << ':' << loc.col();
}

void dumpSourceRange(SourceRange range) {
  auto& o = errs();
  if (!range.valid()) {
    o << "<invalid range>";
    return;
  }

  dumpSourceLoc(range.start());
  o << '-';
  dumpSourceLoc(range.end());
}

} // namespace <anonymous>

void DiagnosticsEngine::Emit(const DiagnosticsMessage& message) {
  auto level = mapDiagLevel(message.level());
  auto& o = errs();

  o << "jvc: " << getDiagLevelName(level) << ": "
    << message.FormatMessage() << '\n';

  if (message.range().valid()) {
    auto sourceFileInfo = _ci.GetSourceManager().GetSourceFileInfo(message.range());
    if (sourceFileInfo) {
      auto indGuard1 = o.PushIndent();

      o << "In file " << sourceFileInfo->path() << ':';
      dumpSourceRange(message.range());
      o << ":\n";

      auto indGuard2 = o.PushIndent();
      auto sourceView = sourceFileInfo->GetViewInRange(message.range());
      o << sourceView << '\n';
    }
  } else if (message.location().valid()) {
    auto sourceFileInfo = _ci.GetSourceManager().GetSourceFileInfo(message.location());
    if (sourceFileInfo) {
      auto indGuard1 = o.PushIndent();

      o << "In file " << sourceFileInfo->path() << ':';
      dumpSourceLoc(message.location());
      o << ":\n";

      auto indGuard2 = o.PushIndent();
      auto sourceView = sourceFileInfo->GetViewAtLoc(message.location());
      o << sourceView << '\n';
    }
  }

  o << '\n';

  if (shouldExit(level)) {
    std::exit(1);
  }
}

DiagnosticsLevel DiagnosticsEngine::mapDiagLevel(DiagnosticsLevel level) const {
  if (level == DiagnosticsLevel::Warning && _opt.TreatWarningsAsErrors) {
    return DiagnosticsLevel::Error;
  } else {
    return level;
  }
}

bool DiagnosticsEngine::shouldExit(DiagnosticsLevel level) const {
  return level == DiagnosticsLevel::Fatal ||
      (level == DiagnosticsLevel::Error && _opt.ExitOnError);
}

} // namespace jvc
