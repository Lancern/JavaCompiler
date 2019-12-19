//
// Created by Sirui Mu on 2019/12/19.
//

#include "Frontend/Diagnostics.h"
#include "Frontend/CompilerInstance.h"

#include <iostream>

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

std::string convertLocToString(SourceLocation loc) {
  if (!loc.valid()) {
    return std::string("<invalid loc>");
  }

  std::string s;
  s.append(std::to_string(loc.row()));
  s.push_back(':');
  s.append(std::to_string(loc.col()));

  return s;
}

std::string convertRangeToString(SourceRange range) {
  if (!range.valid()) {
    return std::string("<invalid range>");
  }

  std::string s;
  s.append(convertLocToString(range.start()));
  s.push_back('-');
  s.append(convertLocToString(range.end()));

  return s;
}

template <typename OutputStream>
OutputStream& dumpText(OutputStream& o, std::string_view text, int indent = 0) {
  const auto npos = std::string_view::npos;
  size_t len;
  while (!text.empty()) {
    len = text.find('\n');
    if (len == npos) {
      len = text.length();
    }

    for (auto i = 0; i < indent; ++i) {
      o << ' ';
    }
    o << text.substr(0, len);

    if (len + 1 > text.length()) {
      break;
    }
    text.remove_prefix(len + 1);
  }

  return o;
}

} // namespace <anonymous>

void DiagnosticsEngine::Emit(const DiagnosticsMessage& message) {
  auto level = mapDiagLevel(message.level());

  std::cerr << "jvc: " << getDiagLevelName(level) << ": "
            << message.FormatMessage() << std::endl;

  if (message.range().valid()) {
    auto sourceFileInfo = _ci.GetSourceManager().GetSourceFileInfo(message.range());
    if (sourceFileInfo) {
      std::cerr << "  In file " << sourceFileInfo->path() << ':'
                << convertRangeToString(message.range()) << ':' << std::endl;
      auto sourceView = sourceFileInfo->GetViewInRange(message.range());
      dumpText(std::cerr, sourceView, 4);
    }
  } else if (message.location().valid()) {
    auto sourceFileInfo = _ci.GetSourceManager().GetSourceFileInfo(message.location());
    if (sourceFileInfo) {
      std::cerr << "  In file " << sourceFileInfo->path() << ':'
                << convertLocToString(message.location()) << std::endl;
      auto sourceView = sourceFileInfo->GetViewAtLoc(message.location());
      dumpText(std::cerr, sourceView, 4);
    }
  }

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
