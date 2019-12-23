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

  void DumpMessage(StreamWriter& output) const override {
    output << _message;
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

} // namespace <anonymous>

void DiagnosticsEngine::Emit(const DiagnosticsMessage& message) {
  auto level = mapDiagLevel(message.level());
  auto& o = errs();

  o << "jvc: " << getDiagLevelName(level) << ": ";
  message.DumpMessage(o);
  o << '\n';

  if (message.range().valid()) {
    auto sourceFileInfo = _ci.GetSourceManager().GetSourceFileInfo(message.range());
    if (sourceFileInfo) {
      auto indGuard1 = o.PushIndent();

      o << "In file " << sourceFileInfo->path() << ':';
      message.range().Dump(o);
      o << ":\n";

      auto indGuard2 = o.PushIndent();
      auto sourceView = sourceFileInfo->GetViewInRange(message.range());
      o << sourceView;
      if (sourceView.back() != '\n') {
        o << '\n';
      }

      if (message.range().start().row() == message.range().end().row()) {
        for (auto i = 1; i < message.range().start().col(); ++i) {
          o << ' ';
        }
        o << '^';
        for (auto i = message.range().start().col() + 1; i < message.range().end().col(); ++i) {
          o << '~';
        }
      }
    }
  } else if (message.location().valid()) {
    auto sourceFileInfo = _ci.GetSourceManager().GetSourceFileInfo(message.location());
    if (sourceFileInfo) {
      auto indGuard1 = o.PushIndent();

      o << "In file " << sourceFileInfo->path() << ':';
      message.location().Dump(o);
      o << ":\n";

      auto indGuard2 = o.PushIndent();
      auto sourceView = sourceFileInfo->GetViewAtLoc(message.location());
      o << sourceView;
      if (sourceView.back() != '\n') {
        o << '\n';
      }

      for (auto i = 1; i < message.range().start().col(); ++i) {
        o << ' ';
      }
      o << '^';
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
