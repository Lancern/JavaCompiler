//
// Created by Sirui Mu on 2019/12/19.
//

#include "Infrastructure/Stream.h"

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

} // namespace anonymous

std::unique_ptr<InputStream> InputStream::FromSTL(std::istream &inner) {
  return std::make_unique<STLInputStreamWrapper>(inner);
}

std::unique_ptr<OutputStream> OutputStream::FromSTL(std::ostream &inner) {
  return std::make_unique<STLOutputStreamWrapper>(inner);
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
