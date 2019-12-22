//
// Created by Sirui Mu on 2019/12/21.
//

#include "Infrastructure/Stream.h"
#include "SourceFileLineBuffer.h"

#include <cassert>
#include <memory>

namespace jvc {

std::unique_ptr<SourceFileInfo::SourceFileLineBuffer>
    SourceFileInfo::SourceFileLineBuffer::Load(std::unique_ptr<InputStream> inputData) {
  assert(inputData && "inputData is nullptr.");

  StreamReader reader { std::move(inputData) };
  auto content = reader.ReadToEnd();

  std::vector<size_t> lineStarts;
  lineStarts.push_back(0);
  for (size_t i = 0; i < content.size(); ++i) {
    if (content[i] == '\n') {
      lineStarts.push_back(i + 1);
    }
  }

  return std::make_unique<SourceFileInfo::SourceFileLineBuffer>(std::move(content), std::move(lineStarts));
}

size_t SourceFileInfo::SourceFileLineBuffer::GetLineWidth(size_t lineNumber) const {
  if (lineNumber == lines()) {
    return length() - _lineStarts.back();
  }

  --lineNumber;
  return _lineStarts[lineNumber + 1] - _lineStarts[lineNumber];
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
