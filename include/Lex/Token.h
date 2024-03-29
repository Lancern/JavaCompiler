//
// Created by Sirui Mu on 2019/12/19.
//

#ifndef JVC_TOKEN_H
#define JVC_TOKEN_H

#include "Frontend/SourceLocation.h"

#include <string>
#include <string_view>

namespace jvc {

class StreamWriter;

#define JVC_TOKEN_KIND_LIST(h) \
    h(Keyword) \
    h(Identifier) \
    h(Literal) \
    h(Delimiter) \
    h(Operator) \
    h(Comment) \
    h(Whitespace)

/**
 * @brief Kinds of lexical tokens.
 *
 */
enum class TokenKind {
#define DEF_VARIANT(v) v,
 JVC_TOKEN_KIND_LIST(DEF_VARIANT)
#undef DEF_VARIANT
};

/**
 * @brief A lexical token generated by the lexer.
 *
 */
class Token {
public:
  /**
   * @brief Destroy the @see Token object.
   *
   */
  virtual ~Token() = default;

  Token(const Token &) = delete;
  Token(Token &&) noexcept = default;

  Token& operator=(const Token &) = delete;
  Token& operator=(Token &&) noexcept = default;

  /**
   * @brief Get the kind of the token.
   *
   * @return the kind of the token.
   */
  [[nodiscard]]
  TokenKind kind() const { return _kind; }

#define GENERATE_IDENTITY_METHOD(v) \
    bool Is##v() const { return kind() == TokenKind::v; }
  JVC_TOKEN_KIND_LIST(GENERATE_IDENTITY_METHOD)
#undef GENERATE_IDENTITY_METHOD

  /**
   * @brief Get the source code range of the token.
   *
   * @return SourceRange the source code range of the token.
   */
  [[nodiscard]]
  SourceRange range() const { return _range; }

  /**
   * @brief Dump current token to the given output stream.
   * @param o output stream writer.
   */
  virtual void Dump(StreamWriter& o) const = 0;

protected:
  /**
   * @brief Initialize a new @see Token object.
   *
   * @param kind the kind of the token.
   * @param range the source code range of the token.
   */
  explicit Token(TokenKind kind, SourceRange range)
    : _kind(kind),
      _range(range)
  { }

private:
  TokenKind _kind;
  SourceRange _range;
}; // class Token

#define JVC_KEYWORD_LIST(h) \
    h(Abstract) \
    h(Boolean) \
    h(Break) \
    h(Byte) \
    h(Case) \
    h(Catch) \
    h(Char) \
    h(Class) \
    h(Const) \
    h(Continue) \
    h(Default) \
    h(Do) \
    h(Double) \
    h(Else) \
    h(Extends) \
    h(False) \
    h(Final) \
    h(Finally) \
    h(Float) \
    h(For) \
    h(Goto) \
    h(If) \
    h(Implements) \
    h(Import) \
    h(Instanceof) \
    h(Int) \
    h(Interface) \
    h(Long) \
    h(Native) \
    h(New) \
    h(Null) \
    h(Package) \
    h(Private) \
    h(Protected) \
    h(Public) \
    h(Return) \
    h(Short) \
    h(Static) \
    h(Super) \
    h(Switch) \
    h(Synchronized) \
    h(This) \
    h(Throw) \
    h(Throws) \
    h(Transient) \
    h(True) \
    h(Try) \
    h(Void) \
    h(Volatile) \
    h(While)

/**
 * @brief Kind of keyword.
 *
 */
enum class KeywordKind {
#define DEF_VARIANT(v) v,
  JVC_KEYWORD_LIST(DEF_VARIANT)
#undef DEF_VARIANT
};

/**
 * @brief Determine whether the given keyword is a type specifier.
 *
 * @param keyword the keyword.
 * @return true if the keyword is a type specifier.
 * @return false if the keyword is not a type specifier.
 */
inline bool IsTypeSpecifier(KeywordKind keyword) {
  return keyword == KeywordKind::Boolean ||
      keyword == KeywordKind::Byte ||
      keyword == KeywordKind::Char ||
      keyword == KeywordKind::Double ||
      keyword == KeywordKind::Float ||
      keyword == KeywordKind::Int ||
      keyword == KeywordKind::Long ||
      keyword == KeywordKind::Short ||
      keyword == KeywordKind::Void;
}

/**
 * @brief Specializes a lexical token that represents a language keyword.
 *
 */
class KeywordToken : public Token {
public:
  /**
   * @brief Initialize a new @see KeywordToken object.
   *
   * @param keyword the keyword type.
   * @param range the range of the keyword token in the source code file.
   */
  explicit KeywordToken(KeywordKind keyword, SourceRange range)
    : Token { TokenKind::Keyword, range },
      _keywordKind(keyword)
  { }

  /**
   * @brief Get the keyword kind.
   *
   * @return the keyword kind.
   */
  [[nodiscard]]
  KeywordKind keywordKind() const { return _keywordKind; }

#define GENERATE_IDENTITY_METHOD(v) \
    bool Is##v() const { return keywordKind() == KeywordKind::v; }
  JVC_KEYWORD_LIST(GENERATE_IDENTITY_METHOD)
#undef GENERATE_IDENTITY_METHOD

  /**
   * @brief Determine whether this keyword is a type specifier.
   *
   * @return true if this keyword is a type specifier.
   * @return false if this keyword is not a type specifier.
   */
  [[nodiscard]]
  bool IsTypeSpecifier() const { return jvc::IsTypeSpecifier(_keywordKind); }

  void Dump(StreamWriter &o) const override;

private:
  KeywordKind _keywordKind;
}; // class KeywordToken

/**
 * @brief Specialize a lexical token that represents an identifier.
 *
 */
class IdentifierToken : public Token {
public:
  /**
   * @brief Initialize a new @see IdentifierToken object.
   *
   * @param name the name of the identifier.
   * @param range the source code range of the identifier.
   */
  explicit IdentifierToken(std::string name, SourceRange range)
    : Token { TokenKind::Identifier, range },
      _name(std::move(name))
  { }

  /**
   * @brief Get the name of the identifier.
   *
   * @return std::string the name of the identifier.
   */
  [[nodiscard]]
  const std::string& name() const { return _name; }

  void Dump(StreamWriter& o) const override;

private:
  std::string _name;
}; // class IdentifierToken

#define JVC_LITERAL_TYPE_LIST(h) \
    h(Number) \
    h(String) \
    h(Character)

/**
 * @brief Kind of literals.
 */
enum class LiteralKind {
#define DEF_VARIANT(v) v,
  JVC_LITERAL_TYPE_LIST(DEF_VARIANT)
#undef DEF_VARIANT
};

/**
 * @brief Specialize a token that represents a string literal or a number literal.
 */
class LiteralToken : public Token {
public:
  /**
   * @brief Get the kind of the literal.
   * @return kind of the literal.
   */
  [[nodiscard]]
  LiteralKind literalKind() const { return _kind; }

#define GENERATE_IDENTITY_METHOD(v) \
    bool Is##v() const { return literalKind() == LiteralKind::v; }
  JVC_LITERAL_TYPE_LIST(GENERATE_IDENTITY_METHOD)
#undef GENERATE_IDENTITY_METHOD

protected:
  /**
   * @brief Initialize a new @see LiteralToken object.
   * @param kind the kind of the literal.
   * @param range the source code range of the literal.
   */
  explicit LiteralToken(LiteralKind kind, SourceRange range)
    : Token { TokenKind::Literal, range },
      _kind(kind)
  { }

private:
  LiteralKind _kind;
};

/**
 * @brief Prefix of number literals.
 */
enum class NumberLiteralPrefix {
  /**
   * @brief No prefixes.
   */
  None,

  /**
   * @brief The '0' prefix.
   */
  Oct,

  /**
   * @brief The '0x' or '0X' prefix.
   */
  Hex
};

/**
 * @brief Suffix of number literals.
 */
enum class NumberLiteralSuffix {
  /**
   * @brief No suffixes.
   */
  None,

  /**
   * @brief The `l` suffix.
   */
  Long,

  /**
   * @brief The `f` suffix.
   */
  Float,
};

/**
 * @brief Specialize a literal token that represents a number literal.
 */
class NumberLiteralToken : public LiteralToken {
public:
  /**
   * @brief Initialize a new @see NumberLiteralToken object that represents an integer literal whose value fits in a
   * 64-bit signed integer.
   * @param source the source of the literal token.
   * @param intValue the literal value in integer representation.
   * @param prefix literal prefix.
   * @param suffix literal suffix.
   * @param range the source code range of this token.
   */
  explicit NumberLiteralToken(
      int64_t intValue, NumberLiteralPrefix prefix, NumberLiteralSuffix suffix, SourceRange range)
    : LiteralToken { LiteralKind::Number, range },
      _isIntValue(true),
      _intValue(intValue),
      _floatValue(static_cast<double>(intValue)),
      _prefix(prefix),
      _suffix(suffix)
  { }

  /**
   * @brief Initialize a new @see NumberLiteralToken object that represents a floating-point number literal.
   * @param intValue the literal value in integer representation.
   * @param prefix literal prefix.
   * @param suffix literal suffix.
   * @param range the source code range of this token.
   */
  explicit NumberLiteralToken(
      double floatValue, NumberLiteralPrefix prefix, NumberLiteralSuffix suffix, SourceRange range)
    : LiteralToken { LiteralKind::Number, range },
      _isIntValue(false),
      _intValue(0),
      _floatValue(floatValue),
      _prefix(prefix),
      _suffix(suffix)
  { }

  /**
   * @brief Determine whether this literal token represents an integer.
   * @return whether this literal token represents an integer.
   */
  [[nodiscard]]
  bool IsInteger() const { return _isIntValue; }

  /**
   * @brief Get 64-bit signed integer representation of this literal token.
   * @return 64-bit signed integer representation of this literal token.
   */
  [[nodiscard]]
  int64_t AsInt64() const { return _intValue; }

  /**
   * @brief Get double precision floating point representation of this literal token.
   * @return double precision floating point representation of this literal token.
   */
  [[nodiscard]]
  double AsDouble() const { return _floatValue; }

  /**
   * @brief Get the prefix of the number literal.
   * @return the prefix of the number literal.
   */
  [[nodiscard]]
  NumberLiteralPrefix prefix() const { return _prefix; }

  /**
   * @brief Get the literal suffix.
   * @return the literal suffix.
   */
  [[nodiscard]]
  NumberLiteralSuffix suffix() const { return _suffix; }

  void Dump(StreamWriter& o) const override;

private:
  bool _isIntValue;
  int64_t _intValue;
  double _floatValue;
  NumberLiteralPrefix _prefix;
  NumberLiteralSuffix _suffix;
};

/**
 * @brief Specialize a literal token that represents a string literal.
 */
class StringLiteralToken : public LiteralToken {
public:
  /**
   * @brief Initialize a new @see StringLiteralToken class.
   * @param source source of this literal token.
   * @param content actual content of this literal token.
   * @param range source range of this literal token.
   */
  explicit StringLiteralToken(std::string source, std::string content, SourceRange range)
    : LiteralToken { LiteralKind::String, range },
      _source(std::move(source)),
      _content(std::move(content))
  { }

  /**
   * @brief Get the source of this literal token.
   * @return the source of this literal token.
   */
  [[nodiscard]]
  const std::string& source() const { return _source; }

  /**
   * @brief Get the actual content of this string literal.
   * @return the actual content of this string literal.
   */
  [[nodiscard]]
  const std::string& content() const { return _content; }

  void Dump(StreamWriter& o) const override;

private:
  std::string _source;
  std::string _content;
};

/**
 * @brief Specialse a literal token that represents a character literal.
 */
class CharacterLiteralToken : public LiteralToken {
public:
  /**
   * @brief Initialize a new @see CharacterLiteralToken object.
   * @param source the source of this token.
   * @param ch the character represented by this token.
   * @param range the source code range of this token.
   */
  explicit CharacterLiteralToken(std::string source, char ch, SourceRange range)
    : LiteralToken { LiteralKind::Character, range },
      _source(std::move(source)),
      _ch(ch)
  { }

  /**
   * @brief Get the source of this token.
   * @return the source of this token.
   */
  [[nodiscard]]
  const std::string& source() const { return _source; }

  /**
   * @brief Get the character value represented by this token.
   * @return the character value represented by this token.
   */
  [[nodiscard]]
  char value() const { return _ch; }

  void Dump(StreamWriter& o) const override;

private:
  std::string _source;
  char _ch;
};

#define JVC_DELIMITER_LIST(h) \
    h(OpenCurlyBrase) \
    h(CloseCurlyBrase) \
    h(OpenBracketBrase) \
    h(CloseBracketBrase) \
    h(OpenParen) \
    h(CloseParen) \
    h(Comma) \
    h(Dot) \
    h(Semicolon) \
    h(At)

/**
 * @brief Kind of delimiter.
 */
enum class DelimiterKind {
#define DEF_VARIANT(v) v,
  JVC_DELIMITER_LIST(DEF_VARIANT)
#undef DEF_VARIANT
};

/**
 * @brief Specialize a token that represents a delimiter.
 */
class DelimiterToken : public Token {
public:
  /**
   * @brief Initialize a new @see DelimiterToken object.
   * @param kind kind of the delimiter.
   * @param range source code range of this delimiter.
   */
  explicit DelimiterToken(DelimiterKind kind, SourceRange range)
    : Token { TokenKind::Delimiter, range },
      _kind(kind)
  { }

  /**
   * @brief Get the kind of this delimiter.
   * @return kind of this delimiter.
   */
  [[nodiscard]]
  DelimiterKind delimiter() const { return _kind; }

#define GENERATE_IDENTITY_METHOD(v) \
    bool Is##v() const { return delimiter() == DelimiterKind::v; }
  JVC_DELIMITER_LIST(GENERATE_IDENTITY_METHOD)
#undef GENERATE_IDENTITY_METHOD

  void Dump(StreamWriter& o) const override;

private:
  DelimiterKind _kind;
};

#define JVC_OPERATOR_LIST(h) \
    h(AddAssignment) \
    h(Add) \
    h(Assignment) \
    h(And) \
    h(AndAssignment) \
    h(Or) \
    h(OrAssignment) \
    h(Xor) \
    h(XorAssignment) \
    h(BitwiseNeg) \
    h(QuationMark) \
    h(Colon) \
    h(Decrement) \
    h(DivideAssignment) \
    h(Divide) \
    h(Equal) \
    h(Greater) \
    h(GreaterOrEqual) \
    h(Increment) \
    h(LeftShift) \
    h(LeftShiftAssignment) \
    h(Less) \
    h(LessOrEqual) \
    h(Modulo) \
    h(ModuloAssignment) \
    h(Multiply) \
    h(MultiplyAssignment) \
    h(Not) \
    h(NotEqual) \
    h(RightShift) \
    h(RightShiftAssignment) \
    h(LogicalAnd) \
    h(LogicalOr) \
    h(SubtractAssignment) \
    h(Subtract) \
    h(UnsignedRightShift) \
    h(UnsignedRightShiftAssignment)

/**
 * @brief Kind of operators.
 */
enum class OperatorKind {
#define DEF_VARIANT(v) v,
  JVC_OPERATOR_LIST(DEF_VARIANT)
#undef DEF_VARIANT
};

/**
 * @brief Specialize a token that represents an operator.
 */
class OperatorToken : public Token {
public:
  /**
   * @brief Initialize a new @see OperatorToken instance.
   * @param kind the kind of operator.
   * @param range the source code range of the operator.
   */
  explicit OperatorToken(OperatorKind kind, SourceRange range)
    : Token { TokenKind::Operator, range },
      _kind(kind)
  { }

  /**
   * @brief Get the kind of the operator.
   * @return kind of the operator.
   */
  [[nodiscard]]
  OperatorKind operatorKind() const { return _kind; }

  void Dump(StreamWriter& o) const override;

private:
  OperatorKind _kind;
};

/**
 * @brief Kind of comment tokens.
 */
enum class CommentKind {
  /**
   * @brief Line comments.
   */
  LineComment,

  /**
   * @brief Block comments.
   */
  BlockComment,
};

/**
 * @brief Specialize a lexical token that represents a comment.
 */
class CommentToken : public Token {
public:
  /**
   * @brief Initialize a new @see CommentToken object.
   * @param content the content of the comment.
   * @param kind kind of this comment.
   * @param range the source code range of the comment.
   */
  explicit CommentToken(std::string content, CommentKind kind, SourceRange range)
    : Token { TokenKind::Comment, range },
      _kind(kind),
      _content(std::move(content))
  { }

  /**
   * @brief Get the kind of this comment token.
   * @return kind of this comment token.
   */
  [[nodiscard]]
  CommentKind commentKind() const { return _kind; }

  /**
   * @brief Get the content of the comment.
   * @return the content of the comment.
   */
  [[nodiscard]]
  const std::string& content() const { return _content; }

  void Dump(StreamWriter& o) const override;

private:
  CommentKind _kind;
  std::string _content;
};

/**
 * @brief Specialize a token that represents a chunk of continuous whitespaces in the source.
 */
class WhitespaceToken : public Token {
public:
  /**
   * @brief Initialize a new @see WhitespaceToken object.
   * @param range the source code range of this token.
   */
  explicit WhitespaceToken(SourceRange range)
    : Token { TokenKind::Whitespace, range }
  { }

  void Dump(StreamWriter& o) const override;
};

} // namespace jvc

#endif // JVC_TOKEN_H
