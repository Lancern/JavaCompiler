//
// Created by Sirui Mu on 2019/12/22.
//

#include "Infrastructure/Stream.h"

#include <memory>

namespace jvc {

std::string StreamReader::ReadToEnd() {
  constexpr const int BufferSize = 4096;
  auto buffer = std::make_unique<char[]>(BufferSize);

  std::string s { };

  size_t read;
  while ((read = _inner->Read(buffer.get(), BufferSize))) {
    s.append(buffer.get(), read);
  }

  return s;
}

} // namespace jvc
