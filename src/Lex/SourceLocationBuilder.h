//
// Created by Sirui Mu on 2019/12/20.
//

#ifndef JVC_SOURCELOCATIONBUILDER_H
#define JVC_SOURCELOCATIONBUILDER_H

#include "Frontend/SourceLocation.h"

namespace jvc {

/**
 * @brief Builder class for @see SourceLocation values.
 */
class SourceLocationBuilder {
public:
  /**
   * @brief Initialize a new @see SourceLocationBuilder class.
   * @param fileId ID of the source code file.
   */
  explicit SourceLocationBuilder(int fileId)
    : _fileId(fileId),
      _row(1),
      _col(1)
  { }

  /**
   * @brief Get the ID of the source code file.
   * @return ID of the source code file.
   */
  [[nodiscard]]
  int fileId() const { return _fileId; }

  /**
   * @brief Get the row number.
   * @return the row number.
   */
  [[nodiscard]]
  int row() const { return _row; }

  /**
   * @brief Get the column number.
   * @return the column number.
   */
  [[nodiscard]]
  int col() const { return _col; }

  /**
   * @brief Build a new @see SourceLocation value based on current state of this object.
   *
   * The returned @see SourceLocation object represents the source position right after the last character given to this
   * object by the @see UpdateState method.
   *
   * @return @see SourceLocation value built.
   */
  [[nodiscard]]
  SourceLocation GetSourceLocation() const { return SourceLocation { _fileId, _row, _col }; }

  /**
   * @brief Update counters inside this @see SourceLocationBuilder object to match the next state transferred to by
   * the given character.
   * @param ch the input character.
   */
  void UpdateState(char ch) {
    if (ch == '\n') {
      ++_row;
      _col = 1;
    } else {
      ++_col;
    }
  }

private:
  int _fileId;
  int _row;
  int _col;
};

} // namespace jvc

#endif // JVC_SOURCELOCATIONBUILDER_H
