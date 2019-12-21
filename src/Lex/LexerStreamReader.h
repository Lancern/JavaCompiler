//
// Created by Sirui Mu on 2019/12/20.
//

#ifndef JVC_LEXERSTREAMREADER_H
#define JVC_LEXERSTREAMREADER_H

#include "Infrastructure/Stream.h"
#include "Lex/Lexer.h"

#include <memory>
#include <optional>

namespace jvc {

class Lexer::LexerStreamReader {
  class LexerStreamReaderBuffer;

public:
  /**
   * @brief Initialize a new @see LexerStreamReader object.
   * @param inner the underlying input stream.
   */
  explicit LexerStreamReader(std::unique_ptr<InputStream> inner);

  /**
   * @brief Destroy this @see LexerStreamReader object.
   */
  ~LexerStreamReader();

  /**
   * @brief Get next character available. This function does not consume the next character.
   * @param ch output parameter, specifying the next character.
   * @return whether the next character is available. This function returns false when EOS has been hit on the
   * underlying input stream.
   */
  bool PeekChar(char& ch);

  /**
   * @brief Get next character available and consume it.
   * @param ch output parameter, specifying the next character.
   * @return whether the next character is available. This function returns false when EOS has been hit on the
   * underlying input stream.
   */
  bool ReadChar(char& ch);

private:
  std::unique_ptr<LexerStreamReaderBuffer> _buffer;
};

// class Lexer::LexerStreamReader

} // namespace jvc

#endif // JVC_LEXERSTREAMREADER_H
