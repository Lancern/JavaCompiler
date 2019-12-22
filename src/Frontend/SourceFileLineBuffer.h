//
// Created by Sirui Mu on 2019/12/21.
//

#ifndef JVC_SOURCEFILELINEBUFFER_H
#define JVC_SOURCEFILELINEBUFFER_H

#include "Frontend/SourceManager.h"

#include <memory>

namespace jvc {

class InputStream;

class SourceFileInfo::SourceFileLineBuffer {
public:
  static std::unique_ptr<SourceFileLineBuffer> Load(std::unique_ptr<InputStream> input);

  explicit SourceFileLineBuffer(std::string content, std::vector<size_t> lineStarts)
      : _content(std::move(content)),
        _lineStarts(std::move(lineStarts))
  { }

  [[nodiscard]]
  size_t lines() const { return _lineStarts.size(); }

  [[nodiscard]]
  std::string_view GetViewInRange(int startRow, int endRow) const;

  [[nodiscard]]
  std::string_view GetLineView(int row) const {
    return GetViewInRange(row, row + 1);
  }

  [[nodiscard]]
  std::string_view content() const { return _content; }

private:
  std::string _content;
  std::vector<size_t> _lineStarts;
};

} // namespace jvc

#endif // JVC_SOURCEFILELINEBUFFER_H
