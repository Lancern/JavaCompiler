//
// Created by Sirui Mu on 2019/12/23.
//

#include "Frontend/SourceLocation.h"
#include "Infrastructure/Stream.h"

namespace jvc {

void SourceLocation::Dump(StreamWriter &output) const {
  if (!valid()) {
    output << "<invalid loc>";
    return;
  }

  output << _row << ':' << _col;
}

void SourceRange::Dump(StreamWriter &output) const {
  if (!valid()) {
    output << "<invalid range>";
    return;
  }

  _start.Dump(output);
  output << ':';
  _end.Dump(output);
}

} // namespace jvc
