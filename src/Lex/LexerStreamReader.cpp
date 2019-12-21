//
// Created by Sirui Mu on 2019/12/20.
//

#include "LexerStreamReader.h"

#include <functional>

namespace jvc {

class Lexer::LexerStreamReader::LexerStreamReaderBuffer {
public:
  explicit LexerStreamReaderBuffer(std::unique_ptr<InputStream> source)
    : _source(std::move(source)),
      _buffer(std::make_unique<char[]>(BufferCapacity)),
      _readPtr(0),
      _bufferSize(0)
  { }

  bool PeekChar(char& ch) {
    if (_readPtr == _bufferSize) {
      loadNextBlock();
      if (_bufferSize == 0) {
        return false;
      }
    }

    ch = _buffer[_readPtr];
    return true;
  }

  bool ReadChar(char& ch) {
    if (!PeekChar(ch)) {
      return false;
    }
    ++_readPtr;
    return true;
  }

private:
  constexpr static const int BufferCapacity = 4096;

  std::unique_ptr<InputStream> _source;
  std::unique_ptr<char[]> _buffer;
  size_t _readPtr;
  size_t _bufferSize;

  void loadNextBlock() {
    _bufferSize = _source->Read(_buffer.get(), BufferCapacity);
    _readPtr = 0;
  }
};

Lexer::LexerStreamReader::LexerStreamReader(std::unique_ptr<InputStream> inner)
  : _buffer(std::make_unique<LexerStreamReaderBuffer>(std::move(inner)))
{ }

Lexer::LexerStreamReader::~LexerStreamReader() = default;

bool Lexer::LexerStreamReader::PeekChar(char &ch) {
  return _buffer->PeekChar(ch);
}

bool Lexer::LexerStreamReader::ReadChar(char &ch) {
  return _buffer->ReadChar(ch);
}

} // namespace jvc
