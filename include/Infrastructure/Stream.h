//
// Created by Sirui Mu on 2019/12/19.
//

#ifndef JVC_STREAM_H
#define JVC_STREAM_H

#include <cstddef>
#include <memory>
#include <iostream>
#include <type_traits>

namespace jvc {

/**
 * @brief Abstract class for input streams.
 */
class InputStream {
public:
  /**
   * @brief Create a @see InputStream wrapper around the given @see std::istream.
   * @param inner the STL input stream.
   * @return a @see std::unique_ptr to a @see InputStream object that behaves as a wrapper around the given STL stream.
   */
  static std::unique_ptr<InputStream> FromSTL(std::istream& inner);

  /**
   * @brief Create a @see InputStream wrapper around the given memory buffer.
   * @param buffer pointer to the memory buffer.
   * @param bufferSize size of the memory buffer, in bytes.
   * @return a @see std::unique_ptr to a @see InputStream object that uses the given buffer as the underlying data
   * source.
   */
  static std::unique_ptr<InputStream> FromBuffer(const void* buffer, size_t bufferSize);

  /**
   * @brief Destroy a @see InputStream object.
   */
  virtual ~InputStream() = default;

  InputStream(const InputStream &) = delete;
  InputStream(InputStream &&) noexcept = default;

  InputStream& operator=(const InputStream &) = delete;
  InputStream& operator=(InputStream &&) noexcept = default;

  /**
   * @brief Read raw data from the input stream into the specified buffer.
   * @param buffer pointer to the output buffer.
   * @param bufferSize size of the buffer, in bytes.
   * @return number of bytes read from the stream. This function should return 0 to indicate that end-of-stream (EOS)
   * has been hit.
   */
  virtual size_t Read(void* buffer, size_t bufferSize) = 0;

protected:
  /**
   * @brief Initialize a new @see InputStream object.
   */
  explicit InputStream() = default;
};

/**
 * @brief Abstract class for output streams.
 */
class OutputStream {
public:
  /**
   * @brief Create an @see OutputStream wrapper around the given @see std::istream.
   * @param inner the STL output stream.
   * @return a @see std::unique_ptr to an @see OnputStream object that behaves as a wrapper around the given STL stream.
   */
  static std::unique_ptr<OutputStream> FromSTL(std::ostream& inner);

  /**
   * @brief Destroy a @see OutputStream object.
   */
  virtual ~OutputStream() = default;

  OutputStream(const OutputStream &) = delete;
  OutputStream(OutputStream &&) noexcept = default;

  OutputStream& operator=(const OutputStream &) = delete;
  OutputStream& operator=(OutputStream &&) noexcept = default;

  /**
   * @brief Write raw data into the output stream.
   * @param buffer pointer to the buffer containing raw data.
   * @param bufferSize size of the buffer, in bytes.
   * @return number of bytes actually written into the output stream.
   */
  virtual size_t Write(const void* buffer, size_t bufferSize) = 0;

protected:
  /**
   * @brief Initialize a new @see OutputStream object.
   */
  explicit OutputStream() = default;
};

class StreamWriter;

/**
 * @brief A RAII type connected to a @see StreamWriter. When an object of this type is destroyed,
 */
class StreamWriterIndentGuard {
public:
  /**
   * @brief Destroy this object and frees a single level of indent on the associated @see StreamWriter object.
   */
  ~StreamWriterIndentGuard();

  StreamWriterIndentGuard(const StreamWriterIndentGuard &) = delete;

  StreamWriterIndentGuard(StreamWriterIndentGuard&& another) noexcept
    : _writer(another._writer)
  { another._writer = nullptr; }

  StreamWriterIndentGuard& operator=(const StreamWriterIndentGuard &) = delete;

  StreamWriterIndentGuard& operator=(StreamWriterIndentGuard&& another) noexcept {
    pop();

    _writer = another._writer;
    another._writer = nullptr;

    return *this;
  }

  /**
   * @brief Manually frees a single level of indent on the associated @see StreamWriter object. This function should be
   * called only once. If this function is called, then the destructor of this object takes no action.
   */
  void pop();

  friend class StreamWriter;

private:
  explicit StreamWriterIndentGuard(StreamWriter* writer)
    : _writer(writer)
  { }

private:
  StreamWriter* _writer;
};

/**
 * @brief Provide formatted output to an underlying @see OutputStream.
 */
class StreamWriter {
public:
  static constexpr const int IndentSpaces = 2;

  /**
   * @brief Initialize a new @see StreamWriter object.
   * @param inner the underlying raw stream.
   */
  explicit StreamWriter(std::unique_ptr<OutputStream> inner)
    : _inner(std::move(inner)),
      _indent(0),
      _atLineStart(false)
  { }

  /**
   * @brief Apply a single level of indent and returns a RAII wrapper that automatically frees the indent when it is
   * destroyed.
   * @return a RAII wrapper that automatically frees the indent when it is destroyed.
   */
  StreamWriterIndentGuard PushIndent();

  friend class StreamWriterIndentGuard;

  /**
   * @brief Get the underlying stream.
   * @return the underlying stream.
   */
  [[nodiscard]]
  OutputStream& stream() const { return *_inner; }

  /**
   * @brief Write a character into the underlying stream.
   * @param ch the character to be written.
   */
  void WriteChar(char ch);

  /**
   * @brief Write a C-style string into the underlying stream.
   * @param s a C-style string to be written.
   */
  void Write(const char* s);

  /**
   * @brief Write a string into the underlying stream.
   * @param s the string to be written.
   */
  void Write(const std::string& s);

  /**
   * @brief Write a string view into the underlying stream.
   * @param s the string view to be written.
   */
  void Write(std::string_view s);

  /**
   * @brief Write the given C-style string into the underlying stream followed by a new line character.
   * @param s the C-style string to be written.
   */
  void WriteLine(const char* s) {
    Write(s);
    WriteChar('\n');
  }

  /**
   * @brief Write the given string into the underlying stream followed by a new line character.
   * @param s the C-style string to be written.
   */
  void WriteLine(const std::string& s) {
    Write(s);
    WriteChar('\n');
  }

  /**
   * @brief Write the given string view into the underlying stream followed by a new line character.
   * @param s the C-style string to be written.
   */
  void WriteLine(std::string_view s) {
    Write(s);
    WriteChar('\n');
  }

private:
  std::unique_ptr<OutputStream> _inner;
  int _indent;
  bool _atLineStart;

  void popIndent();

  void writeIndentOnNecessary();
};

StreamWriter& operator<<(StreamWriter& o, bool b);
StreamWriter& operator<<(StreamWriter& o, char ch);
StreamWriter& operator<<(StreamWriter& o, const char* s);
StreamWriter& operator<<(StreamWriter& o, const std::string& s);
StreamWriter& operator<<(StreamWriter& o, std::string_view s);

#define BUILTIN_FORMATTED_TYPE_LIST(h) \
    h(int) \
    h(long) \
    h(long long) \
    h(unsigned) \
    h(unsigned long) \
    h(unsigned long long) \
    h(float) \
    h(double) \
    h(long double)

#define GENERATE_STREAMWRITER_FORMAT_OPERATOR(t) \
    StreamWriter& operator<<(StreamWriter& o, t v) { return o << std::to_string(v); }
BUILTIN_FORMATTED_TYPE_LIST(GENERATE_STREAMWRITER_FORMAT_OPERATOR)
#undef GENERATE_STREAMWRITER_FORMAT_OPERATOR

/**
 * @brief Get a singleton @see OutputStream object that is tied to the standard output stream of the application.
 * @return a singleton @see OutputStream object that is tied to the standard output stream of the application.
 */
StreamWriter& outs();

/**
 * @brief Get a singleton @see OutputStream object that is tied to the standard error stream of the application.
 * @return a singleton @see OutputStream object that is tied to the standard error stream of the application.
 */
StreamWriter& errs();

} // namespace jvc

#endif // JVC_STREAM_H
