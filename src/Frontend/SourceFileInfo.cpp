//
// Created by Sirui Mu on 2019/12/19.
//

#include "Frontend/SourceManager.h"
#include "Frontend/Diagnostics.h"

#include <cerrno>
#include <cstring>
#include <fstream>
#include <sstream>

namespace jvc {

namespace {

class LoadFileFailedDiagnosticsMessage : public DiagnosticsMessage {
public:
  explicit LoadFileFailedDiagnosticsMessage(const std::string& path, int errorCode)
    : DiagnosticsMessage(DiagnosticsLevel::Fatal),
      _path(path),
      _errorCode(errorCode)
  { }

  [[nodiscard]]
  std::string FormatMessage() const override {
    std::string msg("cannot load source file: ");
    msg.append(_path);
    msg.append(": ");
    msg.append(std::strerror(_errorCode));

    return msg;
  }

private:
  const std::string& _path;
  int _errorCode;
};

} // namespace <anonymous>

class SourceFileInfo::SourceFileLineBuffer {
public:
  static std::unique_ptr<SourceFileLineBuffer> Load(const std::string& path, DiagnosticsEngine& diag);

  explicit SourceFileLineBuffer(std::string content, std::vector<size_t> lineStarts)
      : _content(std::move(content)),
        _lineStarts(std::move(lineStarts))
  { }

  [[nodiscard]]
  size_t lines() const { return _lineStarts.size(); }

  [[nodiscard]]
  std::string_view GetViewInRange(int startRow, int endRow) const {
    --startRow;
    --endRow;

    if (startRow < 0 || startRow >= lines()) {
      return std::string_view { };
    }
    if (endRow < 0 || endRow > lines()) {
      return std::string_view { };
    }
    if (endRow <= startRow) {
      return std::string_view { };
    }

    auto startOffset = _lineStarts[startRow];
    auto v = static_cast<std::string_view>(_content);
    v.remove_prefix(startOffset);

    if (endRow == lines()) {
      return v;
    }

    auto endOffset = _lineStarts[endRow];
    v = v.substr(0, endOffset - startOffset);
    return v;
  }

  [[nodiscard]]
  std::string_view GetLineView(int row) const {
    return GetViewInRange(row, row + 1);
  }

private:
  std::string _content;
  std::vector<size_t> _lineStarts;
};

std::unique_ptr<SourceFileInfo::SourceFileLineBuffer>
    SourceFileInfo::SourceFileLineBuffer::Load(const std::string& path, DiagnosticsEngine& diag) {
  std::ifstream fs { path };
  if (fs.fail()) {
    int errorCode = errno;
    diag.Emit(LoadFileFailedDiagnosticsMessage { path, errorCode });
  }

  // Read the whole content of the source code file into a stringstream.
  std::stringstream contentStream;
  contentStream << fs.rdbuf();
  auto content = contentStream.str();

  std::vector<size_t> lineStarts;
  lineStarts.push_back(0);
  for (size_t i = 0; i < content.size(); ++i) {
    if (content[i] == '\n') {
      lineStarts.push_back(i + 1);
    }
  }

  // The last element of lineStarts should be the total length of the content.
  lineStarts.push_back(content.size());

  return std::make_unique<SourceFileInfo::SourceFileLineBuffer>(std::move(content), std::move(lineStarts));
}

SourceFileInfo::SourceFileInfo(int fileId, std::string path, std::unique_ptr<SourceFileLineBuffer> lineBuffer)
    : _id(fileId),
      _path(std::move(path)),
      _lineBuffer(std::move(lineBuffer))
{ }

std::string_view SourceFileInfo::GetViewInRange(SourceRange range) const {
  if (!range.valid()) {
    return std::string_view { };
  }
  return _lineBuffer->GetViewInRange(range.start().row(), range.end().row() + 1);
}

std::string_view SourceFileInfo::GetViewAtLoc(SourceLocation loc) const {
  if (!loc.valid()) {
    return std::string_view { };
  }
  return _lineBuffer->GetLineView(loc.row());
}

SourceLocation SourceFileInfo::GetEOFLoc() const {
  return SourceLocation { _id, static_cast<int>(_lineBuffer->lines()), 1 };
}

SourceFileInfo SourceFileInfo::Load(int fileId, const std::string& path, DiagnosticsEngine& diag) {
  auto lineBuffer = SourceFileLineBuffer::Load(path, diag);
  return SourceFileInfo { fileId, path, std::move(lineBuffer) };
}

}
