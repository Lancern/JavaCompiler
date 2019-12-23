//
// Created by Sirui Mu on 2019/12/20.
//

#ifndef JVC_LEXER_H
#define JVC_LEXER_H

#include "Infrastructure/Stream.h"
#include "Lex/Token.h"
#include "Lex/SourceLocationBuilder.h"

#include <memory>
#include <optional>

namespace jvc {

class CompilerInstance;

/**
 * @brief Provide options for lexers.
 */
struct LexerOptions {
  /**
   * @brief Should lexer keep comment tokens in its output stream?
   */
  bool KeepComment;

  /**
   * @brief Should lexer keep whitespace tokens in its output stream?
   */
  bool KeepWhitespace;
};

/**
 * @brief Facade of the lexer.
 */
class Lexer {
  class LexerStreamReader;

public:
  /**
   * @brief Create a new @see Lexer object.
   * @param ci the compiler instance
   * @param sourceFileId ID of the source code file.
   * @param options lexer options.
   * @return a @see std::unique_ptr to the created @see Lexer object.
   */
  static std::unique_ptr<Lexer> Create(CompilerInstance& ci, int sourceFileId, LexerOptions options = LexerOptions { });

  Lexer(const Lexer &) = delete;
  Lexer(Lexer &&) noexcept = default;

  Lexer& operator=(const Lexer &) = delete;
  Lexer& operator=(Lexer &&) noexcept = delete;

  /**
   * @brief Destroy this @see Lexer object.
   */
  ~Lexer();

  /**
   * @brief Get the lexer options.
   * @return the lexer options.
   */
  [[nodiscard]]
  LexerOptions& options() { return _options; }

  /**
   * @brief Get the lexer options.
   * @return the lexer options.
   */
  [[nodiscard]]
  const LexerOptions& options() const { return _options; }

  /**
   * @brief Get next token available. This function will not consume the next token.
   * @return the next token available. Returns nullptr if EOS has been hit on the underlying input stream.
   */
  Token* PeekNextToken();

  /**
   * @brief Get next token available and consume it.
   * @return the next token available. Returns an empty @see std::unique_ptr if EOS has been hit on the underlying input
   * stream.
   */
  std::unique_ptr<Token> ReadNextToken();

  /**
   * @brief Get the source code location to which the underlying stream's read pointer refers.
   *
   * Note that this function does not necessarily returns the source location of the lexical token in the peek buffer.
   * It is expected to return the source location referred to by the underlying stream, which should go beyond the
   * location of the lexical token in the peek buffer.
   *
   * @return the source code location to which the underlying stream's read pointer refers.
   */
  [[nodiscard]]
  SourceLocation GetNextLocation() const { return _locBuilder.GetSourceLocation(); }

private:
  /**
   * @brief Initialize a new @see Lexer object.
   * @param ci the compiler instance.
   * @param sourceFileId ID of the source code file.
   * @param reader lexer reader.
   * @param options lexer options.
   */
  explicit Lexer(CompilerInstance& ci, int sourceFileId, std::unique_ptr<LexerStreamReader> reader,
      LexerOptions options = LexerOptions { });

  CompilerInstance& _ci;
  LexerOptions _options;
  SourceLocationBuilder _locBuilder;
  std::unique_ptr<LexerStreamReader> _reader;
  std::unique_ptr<Token> _peekBuffer;

  /**
   * @brief Peek next character from the underlying @see LexerStreamReader object. This function will not update the
   * source location maintained in the lexer.
   * @param ch output parameter, the next character read from the underlying @see LexerStreamReader object.
   * @return whether the operation is successful. This function return false if EOS has been hit on the underlying
   * @see LexerStreamReader object.
   */
  bool peekChar(char& ch);

  template <typename Callback>
  void peekCharOr(char& ch, Callback callback) {
    if (!peekChar(ch)) {
      callback();
    }
  }

  /**
   * @brief Peek next character from the underlying @see LexerStreamReader object. This function will emit an
   * `unexpected EOF` error message if the next char is not available.
   * @return the next character.
   */
  char ensurePeekChar();

  /**
   * @brief Read next character from the underlying @see LexerStreamReader object and maintains the current source
   * location correspondingly.
   * @param ch output parameter, the next character read from the underlying @see LexerStreamReader object.
   * @return whether the operation is successful. This function return false if EOS has been hit on the underlying
   * @see LexerStreamReader object.
   */
  bool readChar(char& ch);

  template <typename Callback>
  void readCharOr(char& ch, Callback callback) {
    if (!readChar(ch)) {
      callback();
    }
  }

  /**
   * @brief Read next character from the underlying @see LexerStreamReader object. This function will emit an
   * `unexpected EOF` error message if the next char is not available.
   * @return the next character.
   */
  char ensureReadChar();

  /**
   * @brief Discard the next character.
   */
  void consumeChar() {
    char ch;
    readChar(ch);
  }

  /**
   * @brief Determine whether the token in the internal peek buffer can be returned to the users of this lexer
   * according to the options.
   * @return whether the token in the internal peek buffer can be returned to the users.
   */
  [[nodiscard]]
  bool shouldKeepCurrentToken() const;

  /**
   * @brief Peek the next lexical token into the internal peek buffer. This function does most of the job of the lexer.
   */
  void peek();

  // The following functions are used by peek during lex to transfer lexer control flow into concrete lexical token
  // types.

  void lexKeywordOrIdentifier(SourceLocation startLoc);
  void lexIdentifier(SourceLocation startLoc);
  void lexStringLiteral(SourceLocation startLoc);
  void lexCharLiteral(SourceLocation startLoc);
  void lexStringLiteralCharacter(std::string& literal, std::string& content);
  void lexStringEscapeSequence(std::string& literal, std::string& content);
  void lexUnicodeCharLiteral(std::string& literal, std::string& content);
  void lexOctCharLiteral(char leader, std::string& literal, std::string& content);
  void lexNumberLiteralOrOperator(SourceLocation startLoc);
  void lexNumberLiteral(SourceLocation startLoc, std::optional<char> sign);
  void lexDelimiter(SourceLocation startLoc);
  void lexOperator(SourceLocation startLoc);
  void lexDivideOperatorOrComment(SourceLocation startLoc);
  void lexComment(SourceLocation startLoc);
  void lexBlockComment(SourceLocation startLoc);
  void lexLineComment(SourceLocation startLoc);
  void lexWhitespace(SourceLocation startLoc);
}; // class Lexer

} // namespace jvc

#endif // JVC_LEXER_H
