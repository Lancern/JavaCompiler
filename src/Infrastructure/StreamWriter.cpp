//
// Created by Sirui Mu on 2019/12/19.
//

#include "Infrastructure/Stream.h"

namespace jvc {

void StreamWriterIndentGuard::pop() {
  if (_writer) {
    _writer->popIndent();
    _writer = nullptr;
  }
}

StreamWriterIndentGuard::~StreamWriterIndentGuard() {
  pop();
}

StreamWriterIndentGuard StreamWriter::PushIndent() {
  _indent += IndentSpaces;
  return StreamWriterIndentGuard { this };
}

void StreamWriter::WriteChar(char ch) {
  if (ch != '\n') {
    // If the character is new line character, no indent should be added no matter where the writer pointer are.
    writeIndentOnNecessary();
  }
  _inner->Write(&ch, 1);

  if (ch == '\n') {
    _atLineStart = true;
  }
}

void StreamWriter::Write(const char *s) {
  Write(std::string_view { s });
}

void StreamWriter::Write(const std::string &s) {
  Write(std::string_view { s });
}

void StreamWriter::Write(std::string_view s) {
  while (!s.empty()) {
    if (s.front() == '\n') {
      WriteChar('\n');
      s.remove_prefix(1);
      continue;
    }

    int window = 0;
    while (s[window] && s[window] != '\n') {
      ++window;
    }

    writeIndentOnNecessary();
    _inner->Write(s.data(), window);
    s.remove_prefix(window);
  }
}

void StreamWriter::popIndent() {
  _indent -= IndentSpaces;
  if (_indent < 0) {
    _indent = 0;
  }
}

void StreamWriter::writeIndentOnNecessary() {
  if (_indent && _atLineStart) {
    char buffer[4];
    const auto bufSize = sizeof(buffer);
    memset(buffer, ' ', sizeof(buffer));

    int i;
    for (i = 0; i + bufSize < _indent; i += bufSize) {
      _inner->Write(buffer, sizeof(buffer));
    }
    while (i++ < _indent) {
      _inner->Write(" ", 1);
    }
  }

  if (_atLineStart) {
    _atLineStart = false;
  }
}

StreamWriter& operator<<(StreamWriter& o, bool b) {
  if (b) {
    o.Write("true");
  } else {
    o.Write("false");
  }
  return o;
}

StreamWriter& operator<<(StreamWriter& o, char ch) {
  o.WriteChar(ch);
  return o;
}

StreamWriter& operator<<(StreamWriter& o, const char *s) {
  o.Write(s);
  return o;
}

StreamWriter& operator<<(StreamWriter& o, const std::string& s) {
  o.Write(s);
  return o;
}

StreamWriter& operator<<(StreamWriter& o, std::string_view s) {
  o.Write(s);
  return o;
}

} // namespace jvc
