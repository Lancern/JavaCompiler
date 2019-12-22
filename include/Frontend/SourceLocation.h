//
// Created by Sirui Mu on 2019/12/19.
//

#ifndef JVC_SOURCELOCATION_H
#define JVC_SOURCELOCATION_H

#include <cassert>

namespace jvc {

/**
 * @brief Provide a handle for a location in the source code. This class is designed to be small enough to be copied
 * efficiently.
 */
class SourceLocation {
public:
  /**
   * @brief Initialize a new @class SourceLocation object.
   */
  explicit SourceLocation()
      : _fileId(InvalidFileId),
        _row(0),
        _col(0)
  { }

  /**
   * @brief Initialize a new @class SourceLocation object.
   * @param fileId the ID of the source code file.
   * @param row the row number of the specified location.
   * @param col the column number of the specified location.
   */
  explicit SourceLocation(int fileId, int row, int col)
      : _fileId(fileId),
        _row(row),
        _col(col)
  { }

  /**
   * @brief Determine whether the current @class SourceLocation object is valid.
   * @return whether the current @class SourceLocation object is valid.
   */
  [[nodiscard]]
  bool valid() const { return _fileId != InvalidFileId; }

  /**
   * @brief Get the ID of the source code file.
   * @return the ID of the source code file.
   */
  [[nodiscard]]
  int fileId() const { return _fileId; }

  /**
   * @brief Get the row number of the source location.
   * @return the row number of the source location.
   */
  [[nodiscard]]
  int row() const { return _row; }

  /**
   * @brief Get the column number of the source location.
   * @return the column number of the source location.
   */
  [[nodiscard]]
  int col() const { return _col; }

  friend bool operator==(const SourceLocation &, const SourceLocation &);

private:
  static constexpr const int InvalidFileId = -1;

  int _fileId;
  int _row;
  int _col;
};

inline bool operator==(const SourceLocation& lhs, const SourceLocation& rhs) {
  if (!lhs.valid() && !rhs.valid()) {
    return true;
  }

  return lhs._fileId == rhs._fileId &&
      lhs._row == rhs._row &&
      lhs._col == rhs._col;
}

inline bool operator!=(const SourceLocation& lhs, const SourceLocation& rhs) {
  return !(lhs == rhs);
}

/**
 * @brief Represent a literal range in some source code file.
 */
class SourceRange {
public:
  /**
   * @brief Initialize a new @class SourceRange that represents an invalid range.
   */
  explicit SourceRange() = default;

  /**
   * @brief Initialize a new @class SourceRange using the start location and the end location of the range.
   *
   * The initialized range is half-open, with the start location included and the end location excluded.
   *
   * @param start the start location of the range, inclusive.
   * @param end the end location of the range, exclusive.
   */
  explicit SourceRange(SourceLocation start, SourceLocation end)
      : _start(start),
        _end(end)
  {
    assert(start.valid() && "start location is invalid");
    assert(end.valid() && "end location is invalid");
  }

  /**
   * @brief Get the start location of the range, inclusive.
   * @return the start location of the range, inclusive.
   */
  [[nodiscard]]
  SourceLocation start() const { return _start; }

  /**
   * @brief Get the end location of the range, exclusive.
   * @return the end location of the range, exclusive.
   */
  [[nodiscard]]
  SourceLocation end() const { return _end; }

  /**
   * @brief Get the ID of the source code file this source range refers to.
   * @return the ID of the source code file this source range refers to.
   */
  [[nodiscard]]
  int fileId() const { return _start.fileId(); }

  /**
   * @brief Determines whether the current @class SourceRange is valid.
   * @return whether the current @class SourceRange is valid.
   */
  [[nodiscard]]
  bool valid() const { return _start.valid() && _end.valid() && _start.fileId() == _end.fileId(); }

  friend bool operator==(const SourceRange &, const SourceRange &);

private:
  SourceLocation _start;
  SourceLocation _end;
};

inline bool operator==(const SourceRange& lhs, const SourceRange& rhs) {
  if (!lhs.valid() && !rhs.valid()) {
    return true;
  }

  return lhs._start == rhs._start &&
      lhs._end == rhs._end;
}

inline bool operator!=(const SourceRange& lhs, const SourceRange& rhs) {
  return !(lhs == rhs);
}

}

#endif // JVC_SOURCELOCATION_H
