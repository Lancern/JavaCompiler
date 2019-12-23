//
// Created by Sirui Mu on 2019/12/19.
//

#include "Infrastructure/Stream.h"

#include <algorithm>
#include <fstream>
#include <type_traits>

namespace jvc {

namespace {

class STLInputStreamWrapper : public InputStream {
public:
  explicit STLInputStreamWrapper(std::istream& inner)
    : _inner(inner)
  { }

  size_t Read(void *buffer, size_t bufferSize) override {
    _inner.read(reinterpret_cast<char *>(buffer), bufferSize);
    return _inner.gcount();
  }

private:
  std::istream& _inner;
};

class MemoryInputStream : public InputStream {
public:
  explicit MemoryInputStream(const void* buffer, size_t bufferSize)
    : _buffer(buffer),
      _bufferSize(bufferSize),
      _readPtr(0)
  { }

  size_t Read(void *buffer, size_t bufferSize) override {
    auto copySize = std::min(bufferSize, _bufferSize - _readPtr);
    memcpy(buffer, reinterpret_cast<const char *>(_buffer) + _readPtr, copySize);
    _readPtr += copySize;
    return copySize;
  }

private:
  const void* _buffer;
  size_t _bufferSize;
  size_t _readPtr;
};

class STLOutputStreamWrapper : public OutputStream {
public:
  explicit STLOutputStreamWrapper(std::ostream& inner)
    : _inner(inner)
  { }

  size_t Write(const void *buffer, size_t bufferSize) override {
    _inner.write(reinterpret_cast<const char *>(buffer), bufferSize);
    // TODO: Should we return bufferSize directly from this function?
    return bufferSize;
  }

private:
  std::ostream& _inner;
};

template <typename Inner>
class STLOwnedOutputStream : public OutputStream {
  static_assert(std::is_base_of_v<std::ostream, Inner>, "Inner does not derive from std::ostream.");

public:
  explicit STLOwnedOutputStream(Inner inner)
    : _wrapper(inner),
      _inner(std::move(inner))
  { }

  size_t Write(const void *buffer, size_t bufferSize) override {
    return _wrapper.Write(buffer, bufferSize);
  }

private:
  STLOutputStreamWrapper _wrapper;
  Inner _inner;
};

} // namespace anonymous

std::unique_ptr<InputStream> InputStream::FromSTL(std::istream &inner) {
  return std::make_unique<STLInputStreamWrapper>(inner);
}

std::unique_ptr<InputStream> InputStream::FromBuffer(const void *buffer, size_t bufferSize) {
  return std::make_unique<MemoryInputStream>(buffer, bufferSize);
}

std::unique_ptr<OutputStream> OutputStream::FromSTL(std::ostream &inner) {
  return std::make_unique<STLOutputStreamWrapper>(inner);
}

std::unique_ptr<OutputStream> OutputStream::FromFile(const std::string& filename) {
  std::ofstream fs { filename };
  if (fs.fail()) {
    return nullptr;
  }

  return std::make_unique<STLOwnedOutputStream<decltype(fs)>>(std::move(fs));
}

namespace {

std::unique_ptr<StreamWriter> stdoutWrapper;
std::unique_ptr<StreamWriter> stderrWrapper;

} // namespace anonymous

StreamWriter& outs() {
  if (!stdoutWrapper) {
    stdoutWrapper = std::make_unique<StreamWriter>(OutputStream::FromSTL(std::cout));
  }
  return *stdoutWrapper;
}

StreamWriter& errs() {
  if (!stderrWrapper) {
    stderrWrapper = std::make_unique<StreamWriter>(OutputStream::FromSTL(std::cerr));
  }
  return *stderrWrapper;
}

} // namespace jvc
