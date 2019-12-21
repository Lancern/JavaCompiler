//
// Created by Sirui Mu on 2019/12/21.
//

#include "Infrastructure/Stream.h"
#include "SourceFileLineBuffer.h"

#include <fstream>
#include <sstream>
#include <memory>

namespace jvc {

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

std::string_view SourceFileInfo::SourceFileLineBuffer::GetViewInRange(int startRow, int endRow) const {
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

} // namespace jvc
