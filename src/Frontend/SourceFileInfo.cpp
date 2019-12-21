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
  return _lineBuffer->GetLineView(loc.row());
}

std::string_view SourceFileInfo::GetContent() const {
  return _lineBuffer->content();
}

SourceLocation SourceFileInfo::GetEOFLoc() const {
  return SourceLocation { _id, static_cast<int>(_lineBuffer->lines()), 1 };
}

std::unique_ptr<InputStream> SourceFileInfo::CreateInputStream() const {
  auto view = _lineBuffer->content();
  return InputStream::FromBuffer(view.data(), view.size());
}

SourceFileInfo SourceFileInfo::Load(int fileId, const std::string& path, DiagnosticsEngine& diag) {
  auto lineBuffer = SourceFileLineBuffer::Load(path, diag);
  return SourceFileInfo { fileId, path, std::move(lineBuffer) };
}

}
