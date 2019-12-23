//
// Created by Sirui Mu on 2019/12/19.
//

#include "Infrastructure/Stream.h"
#include "Frontend/SourceManager.h"
#include "Frontend/Diagnostics.h"
#include "SourceFileLineBuffer.h"

#include <cerrno>
#include <cstring>
#include <fstream>
#include <sstream>

namespace jvc {

SourceFileInfo::SourceFileInfo(int fileId, std::string path, std::unique_ptr<SourceFileLineBuffer> lineBuffer)
    : _id(fileId),
      _path(std::move(path)),
      _lineBuffer(std::move(lineBuffer))
{ }

SourceFileInfo::SourceFileInfo(SourceFileInfo &&) noexcept = default;

SourceFileInfo& SourceFileInfo::operator=(SourceFileInfo &&) noexcept = default;

SourceFileInfo::~SourceFileInfo() = default;

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
  if (loc.fileId() != _id) {
    return std::string_view { };
  }
  return _lineBuffer->GetLineView(loc.row());
}

const std::string& SourceFileInfo::GetContent() const {
  return _lineBuffer->content();
}

SourceLocation SourceFileInfo::GetEOFLoc() const {
  auto lines = static_cast<int>(_lineBuffer->lines());
  auto lastLineWidth = static_cast<int>(_lineBuffer->GetLineWidth(lines));
  return SourceLocation { _id, lines, lastLineWidth + 1 };
}

std::unique_ptr<InputStream> SourceFileInfo::CreateInputStream() const {
  const auto& view = _lineBuffer->content();
  return InputStream::FromBuffer(view.data(), view.size());
}

namespace {

class LoadFileFailedDiagnosticsMessage : public DiagnosticsMessage {
public:
  explicit LoadFileFailedDiagnosticsMessage(const std::string& path, int errorCode)
      : DiagnosticsMessage(DiagnosticsLevel::Fatal),
        _path(path),
        _errorCode(errorCode)
  { }

  void DumpMessage(StreamWriter &output) const override {
    output << "cannot load source file: "
           << _path << ": "
           << std::strerror(_errorCode);
  }

private:
  const std::string& _path;
  int _errorCode;
};

} // namespace <anonymous>

SourceFileInfo SourceFileInfo::Load(int fileId, const std::string& path, DiagnosticsEngine& diag) {
  std::ifstream fs { path };
  if (fs.fail()) {
    int errorCode = errno;
    diag.Emit(LoadFileFailedDiagnosticsMessage { path, errorCode });
  }

  return Load(fileId, path, InputStream::FromSTL(fs));
}

SourceFileInfo SourceFileInfo::Load(int fileId, const std::string& path, std::unique_ptr<InputStream> inputData) {
  auto lineBuffer = SourceFileLineBuffer::Load(std::move(inputData));
  return SourceFileInfo { fileId, path, std::move(lineBuffer) };
}

}
