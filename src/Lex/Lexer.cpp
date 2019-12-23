//
// Created by Sirui Mu on 2019/12/20.
//

#include "Frontend/CompilerInstance.h"
#include "Frontend/SourceLocation.h"
#include "Lex/Lexer.h"
#include "Lex/Token.h"
#include "LexerStreamReader.h"

#include <cctype>
#include <cmath>
#include <unordered_map>
#include <type_traits>
#include <limits>

namespace jvc {

Lexer::Lexer(CompilerInstance& ci, int sourceFileId, std::unique_ptr<LexerStreamReader> reader, LexerOptions options)
  : _ci(ci),
    _options(options),
    _locBuilder { sourceFileId },
    _reader(std::move(reader)),
    _peekBuffer(nullptr)
{ }

Lexer::~Lexer() = default;

std::unique_ptr<Lexer> Lexer::Create(CompilerInstance& ci, int sourceFileId, LexerOptions options) {
  auto sourceFile = ci.GetSourceManager().GetSourceFileInfo(sourceFileId);
  if (!sourceFile) {
    return nullptr;
  }

  auto inputStream = sourceFile->CreateInputStream();
  auto reader = std::make_unique<LexerStreamReader>(std::move(inputStream));

  // We cannot use std::make_unique because constructor of Lexer is private. This is not a problem since the
  // constructor of Lexer should not throw any exceptions.
  return std::unique_ptr<Lexer> { new Lexer(ci, sourceFileId, std::move(reader), options) };
}

Token *Lexer::PeekNextToken() {
  if (!_peekBuffer) {
    peek();
  }
  while (_peekBuffer && !shouldKeepCurrentToken()) {
    peek();
  }
  return _peekBuffer.get();
}

std::unique_ptr<Token> Lexer::ReadNextToken() {
  if (!PeekNextToken()) {
    return nullptr;
  }
  return std::move(_peekBuffer);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCSimplifyInspection"
bool Lexer::shouldKeepCurrentToken() const {
  assert(_peekBuffer && "peek buffer inside lexer is empty");
  if (!_options.KeepWhitespace && _peekBuffer->IsWhitespace()) {
    return false;
  }
  if (!_options.KeepComment && _peekBuffer->IsComment()) {
    return false;
  }

  return true;
}
#pragma clang diagnostic pop

bool Lexer::peekChar(char &ch) {
  return _reader->PeekChar(ch);
}

char Lexer::ensurePeekChar() {
  char ch;
  peekCharOr(ch, [this] {
    auto loc = GetNextLocation();
    auto diagMsg = DiagnosticsMessage::CreateLiteral(DiagnosticsLevel::Error, loc, "Unexpected end-of-file.");
    _ci.GetDiagnosticsEngine().Emit(*diagMsg);
  });

  return ch;
}

bool Lexer::readChar(char &ch) {
  if (!_reader->ReadChar(ch)) {
    return false;
  }
  _locBuilder.UpdateState(ch);
  return true;
}

char Lexer::ensureReadChar() {
  char ch;
  readCharOr(ch, [this] {
    auto loc = GetNextLocation();
    auto diagMsg = DiagnosticsMessage::CreateLiteral(DiagnosticsLevel::Error, loc, "Unexpected end-of-file.");
    _ci.GetDiagnosticsEngine().Emit(*diagMsg);
  });

  return ch;
}

void Lexer::peek() {
  auto startLoc = GetNextLocation();

  char ch;
  if (!peekChar(ch)) {
    _peekBuffer = nullptr;
    return;
  }

  if (std::isspace(ch)) {
    lexWhitespace(startLoc);
    return;
  }

  if (std::isalpha(ch)) {
    lexKeywordOrIdentifier(startLoc);
    return;
  }

  if (ch == '_' || ch == '$') {
    lexIdentifier(startLoc);
    return;
  }

  if (std::isdigit(ch)) {
    lexNumberLiteral(startLoc, std::optional<char> { });
    return;
  }

  if (ch == '\'') {
    lexCharLiteral(startLoc);
    return;
  }

  if (ch == '\"') {
    lexStringLiteral(startLoc);
    return;
  }

  if (ch == '.' || ch == '{' || ch == '}' || ch == '[' || ch == ']' || ch == ',' || ch == '(' || ch == ')' ||
      ch == ';' || ch == '@') {
    lexDelimiter(startLoc);
    return;
  }

  if (ch == '&' || ch == '=' || ch == '~' || ch == '|' || ch == '^' || ch == '?' || ch == ':' || ch == '>' ||
      ch == '<' || ch == '%' || ch == '*' || ch == '!') {
    lexOperator(startLoc);
    return;
  }

  if (ch == '+' || ch == '-') {
    lexNumberLiteralOrOperator(startLoc);
    return;
  }

  if (ch == '/') {
    lexDivideOperatorOrComment(startLoc);
    return;
  }

  auto diagMsg = DiagnosticsMessage::CreateLiteral(DiagnosticsLevel::Error, startLoc, "Unrecognized token");
  _ci.GetDiagnosticsEngine().Emit(*diagMsg);
}

namespace {

const std::unordered_map<std::string, KeywordKind> Keywords = {
    {"abstract",     KeywordKind::Abstract},
    {"boolean",      KeywordKind::Boolean},
    {"break",        KeywordKind::Break},
    {"byte",         KeywordKind::Byte},
    {"case",         KeywordKind::Case},
    {"catch",        KeywordKind::Catch},
    {"char",         KeywordKind::Char},
    {"class",        KeywordKind::Class},
    {"const",        KeywordKind::Const},
    {"continue",     KeywordKind::Continue},
    {"default",      KeywordKind::Default},
    {"do",           KeywordKind::Do},
    {"double",       KeywordKind::Double},
    {"else",         KeywordKind::Else},
    {"extends",      KeywordKind::Extends},
    {"false",        KeywordKind::False},
    {"final",        KeywordKind::Final},
    {"finally",      KeywordKind::Finally},
    {"float",        KeywordKind::Float},
    {"for",          KeywordKind::For},
    {"goto",         KeywordKind::Goto},
    {"if",           KeywordKind::If},
    {"implements",   KeywordKind::Implements},
    {"import",       KeywordKind::Import},
    {"instanceof",   KeywordKind::Instanceof},
    {"int",          KeywordKind::Int},
    {"interface",    KeywordKind::Interface},
    {"long",         KeywordKind::Long},
    {"native",       KeywordKind::Native},
    {"new",          KeywordKind::New},
    {"null",         KeywordKind::Null},
    {"package",      KeywordKind::Package},
    {"private",      KeywordKind::Private},
    {"protected",    KeywordKind::Protected},
    {"public",       KeywordKind::Public},
    {"return",       KeywordKind::Return},
    {"short",        KeywordKind::Short},
    {"static",       KeywordKind::Static},
    {"super",        KeywordKind::Super},
    {"switch",       KeywordKind::Switch},
    {"synchronized", KeywordKind::Synchronized},
    {"this",         KeywordKind::This},
    {"throw",        KeywordKind::Throw},
    {"throws",       KeywordKind::Throws},
    {"transient",    KeywordKind::Transient},
    {"true",         KeywordKind::True},
    {"try",          KeywordKind::Try},
    {"void",         KeywordKind::Void},
    {"volatile",     KeywordKind::Volatile},
    {"while",        KeywordKind::While}
};

} // namespace anonymous

void Lexer::lexKeywordOrIdentifier(SourceLocation startLoc) {
  auto mustBeIdentifier = false;
  std::string literal;

  char ch;
  readChar(ch);
  literal.push_back(ch);

  while (peekChar(ch)) {
    if (std::isalpha(ch) || std::isdigit(ch) || ch == '_' || ch == '$') {
      literal.push_back(ch);
      if (std::isdigit(ch) || ch == '_') {
        mustBeIdentifier = true;
      }
      // Consume this character.
      readChar(ch);
    } else {
      break;
    }
  }

  auto endLoc = GetNextLocation();
  SourceRange range { startLoc, endLoc };
  if (mustBeIdentifier) {
    _peekBuffer = std::make_unique<IdentifierToken>(std::move(literal), range);
    return;
  }

  // Determine whether literal is a keyword.
  auto i = Keywords.find(literal);
  if (i != Keywords.end()) {
    _peekBuffer = std::make_unique<KeywordToken>(i->second, range);
    return;
  }

  _peekBuffer = std::make_unique<IdentifierToken>(std::move(literal), range);
}

void Lexer::lexIdentifier(SourceLocation startLoc) {
  std::string name;

  auto ch = ensureReadChar();
  assert((std::isalpha(ch) || ch == '_' || ch == '$') &&
      "next character is not as expected to be the start of an identifier.");
  name.push_back(ch);

  while (peekChar(ch)) {
    if (std::isalpha(ch) || std::isdigit(ch) || ch == '_') {
      name.push_back(ch);
      consumeChar();
    } else {
      break;
    }
  }

  auto endLoc = GetNextLocation();
  SourceRange range { startLoc, endLoc };
  _peekBuffer = std::make_unique<IdentifierToken>(std::move(name), range);
}

void Lexer::lexStringLiteral(SourceLocation startLoc) {
  std::string literal;
  std::string content;

  auto ch = ensureReadChar();
  assert(ch == '\"' && "next character is not as expected to be the start of a string literal.");
  literal.push_back(ch);

  auto closed = false;
  while (peekChar(ch)) {
    if (ch == '\"') {
      literal.push_back(ch);
      consumeChar();
      closed = true;
      break;
    } else {
      lexStringLiteralCharacter(literal, content);
    }
  }

  auto endLoc = GetNextLocation();
  SourceRange range { startLoc, endLoc };

  if (!closed) {
    auto diagMsg = DiagnosticsMessage::CreateLiteral(DiagnosticsLevel::Error, range, "Unclosed string literal.");
    _ci.GetDiagnosticsEngine().Emit(*diagMsg);
    return;
  }

  _peekBuffer = std::make_unique<StringLiteralToken>(std::move(literal), std::move(content), range);
}

namespace {

class UnexpectedCharDiagnosticsMessage : public DiagnosticsMessage {
public:
  explicit UnexpectedCharDiagnosticsMessage(char expected, char actual, SourceLocation loc)
    : DiagnosticsMessage { DiagnosticsLevel::Error, loc },
      _expected(expected),
      _actual(actual)
  { }

  void DumpMessage(StreamWriter &output) const override {
    output << "Unexpected input character: expected `" << _expected << '`'
           << ", but found `" << _actual << '`';
  }

private:
  char _expected;
  char _actual;
};

} // namespace <anonymous>

void Lexer::lexCharLiteral(SourceLocation startLoc) {
  std::string literal;
  std::string content;

  char ch;
  readChar(ch);
  assert(ch == '\'' && "next character is not as expected to be the start of a char literal.");
  literal.push_back(ch);

  lexStringLiteralCharacter(literal, content);

  ch = ensurePeekChar();
  if (ch != '\'') {
    auto loc = GetNextLocation();
    UnexpectedCharDiagnosticsMessage diagMsg { '\'', ch, loc };
    _ci.GetDiagnosticsEngine().Emit(diagMsg);
    return;
  }
  consumeChar();

  auto endLoc = GetNextLocation();
  SourceRange range { startLoc, endLoc };
  _peekBuffer = std::make_unique<CharacterLiteralToken>(std::move(literal), content[0], range);
}

void Lexer::lexStringLiteralCharacter(std::string& literal, std::string& content) {
  auto ch = ensurePeekChar();
  if (ch == '\\') {
    lexStringEscapeSequence(literal, content);
  } else {
    literal.push_back(ch);
    content.push_back(ch);
    consumeChar();
  }
}

namespace {

class UnknownEscapeSequenceDiagnosticsMessage : public DiagnosticsMessage {
public:
  explicit UnknownEscapeSequenceDiagnosticsMessage(char leader, SourceLocation startLoc)
    : DiagnosticsMessage { DiagnosticsLevel::Error, startLoc },
      _leader(leader)
  { }

  void DumpMessage(StreamWriter &output) const override {
    output << "Unknown escape sequence: `\\" << _leader << '`';
  }

private:
  char _leader;
};

} // namespace anonymous

void Lexer::lexStringEscapeSequence(std::string& literal, std::string& content) {
  auto ch = ensurePeekChar();
  assert(ch == '\\' && "next character is not as expected to be the start of an escape sequence.");

  auto startLoc = GetNextLocation();

  literal.push_back(ch);
  consumeChar();

  ch = ensureReadChar();
  literal.push_back(ch);
  switch (ch) {
    case 'n':
      content.push_back('\n');
      break;

    case 't':
      content.push_back('\t');
      break;

    case 'r':
      content.push_back('\r');
      break;

    case 'f':
      content.push_back('\f');
      break;

    case 'b':
      content.push_back('\b');
      break;

    case '\'':
      content.push_back('\'');
      break;

    case '\\':
      content.push_back('\\');
      break;

    case 'u':
      lexUnicodeCharLiteral(literal, content);
      break;

    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
      lexOctCharLiteral(ch, literal, content);
      break;

    default: {
      UnknownEscapeSequenceDiagnosticsMessage diagMsg { ch, startLoc };
      _ci.GetDiagnosticsEngine().Emit(diagMsg);
    }
  }
}

namespace {

bool isHex(char ch) {
  return (ch >= '0' && ch <= '9') ||
      (ch >= 'A' && ch <= 'F') ||
      (ch >= 'a' && ch <= 'f');
}

bool isOct(char ch) {
  return ch >= '0' && ch <= '7';
}

unsigned parseHex(char ch) {
  if (ch >= '0' && ch <= '9') {
    return ch - '0';
  } else if (ch >= 'a' && ch <= 'f') {
    return ch - 'a' + 10;
  } else if (ch >= 'A' && ch <= 'F') {
    return ch - 'A' + 10;
  } else {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCSimplifyInspection"
    assert(false && "invalid hex digit.");
#pragma clang diagnostic pop
  }
}

unsigned parseHex(const char* s) {
  unsigned value = 0;
  while (*s) {
    value = (value << 4u) | parseHex(*s++);
  }

  return value;
}

unsigned parseOct(const char* s) {
  unsigned value = 0;
  while (*s) {
    auto ch = *s++;
    assert(isOct(ch) && "invalid oct character.");
    value = (value << 3u) | static_cast<unsigned>(ch - '0');
  }

  return value;
}

} // namespace <anonymous>

void Lexer::lexUnicodeCharLiteral(std::string& literal, std::string& content) {
  std::string raw;
  raw.reserve(4);

  raw.push_back(ensureReadChar());
  for (auto i = 0; i < 3; ++i) {
    char ch;
    if (peekChar(ch) && isHex(ch)) {
      literal.push_back(ch);
      raw.push_back(ch);
      consumeChar();
    } else {
      break;
    }
  }

  auto value = parseHex(raw.c_str());
  content.push_back(static_cast<char>(value & 0xFFu));
  if (value & 0xFF00u) {
    content.push_back(static_cast<char>((value & 0xFF00u) >> 8u));
  }
}

void Lexer::lexOctCharLiteral(char leader, std::string& literal, std::string& content) {
  std::string raw;
  raw.reserve(3);
  raw.push_back(leader);

  for (auto i = 0; i < 2; ++i) {
    char ch;
    if (peekChar(ch) && isOct(ch)) {
      literal.push_back(ch);
      raw.push_back(ch);
      consumeChar();
    }
  }

  auto value = parseOct(raw.c_str());
  content.push_back(static_cast<char>(value & 0xFFu));
  if (value & 0xFF00u) {
    content.push_back(static_cast<char>((value & 0xFF00u) >> 8u));
  }
}

void Lexer::lexNumberLiteralOrOperator(SourceLocation startLoc) {
  auto ch = ensureReadChar();
  assert((ch == '+' || ch == '-') &&
      "next character is not as expected to be the start of a number literal or an operator.");

  char nextChar;
  if (peekChar(nextChar)) {
    if (std::isdigit(nextChar)) {
      lexNumberLiteral(startLoc, ch);
      return;
    } else { // nextChar is not a digit
      if (nextChar == '=' || nextChar == ch) {
        consumeChar();
        auto endLoc = GetNextLocation();
        SourceRange range { startLoc, endLoc };

        OperatorKind kind;
        if (ch == '+') {
          if (nextChar == '=') {
            kind = OperatorKind::AddAssignment;
          } else { // nextChar == '+'
            kind = OperatorKind::Increment;
          }
        } else { // ch == '-'
          if (nextChar == '=') {
            kind = OperatorKind::SubtractAssignment;
          } else { // nextChar == '-'
            kind = OperatorKind::Decrement;
          }
        }

        _peekBuffer = std::make_unique<OperatorToken>(kind, range);
        return;
      }
    }
  }

  auto endLoc = GetNextLocation();
  SourceRange range { startLoc, endLoc };
  _peekBuffer = std::make_unique<OperatorToken>(ch == '+' ? OperatorKind::Add : OperatorKind::Subtract, range);
}

namespace {

int getBase(NumberLiteralPrefix prefix) {
  switch (prefix) {
    case NumberLiteralPrefix::None:
      return 10;
    case NumberLiteralPrefix::Oct:
      return 8;
    case NumberLiteralPrefix::Hex:
      return 16;
    default:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCSimplifyInspection"
      assert(false && "invalid literal prefix.");
#pragma clang diagnostic pop
  }
}

bool isDigitUnderPrefix(char ch, NumberLiteralPrefix prefix) {
  switch (prefix) {
    case NumberLiteralPrefix::None:
      return std::isdigit(ch);
    case NumberLiteralPrefix::Oct:
      return isOct(ch);
    case NumberLiteralPrefix::Hex:
      return isHex(ch);
    default:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCSimplifyInspection"
      assert(false && "invalid literal prefix.");
#pragma clang diagnostic pop
  }
}

template <typename IntegralT,
    typename std::enable_if<std::is_integral_v<IntegralT> && std::is_signed_v<IntegralT>, int>::type = 0>
bool willAddOverflowOrUnderflow(IntegralT lhs, IntegralT rhs) {
  if (lhs >= 0) {
    auto maxRhs = std::numeric_limits<IntegralT>::max() - lhs;
    if (rhs > maxRhs) {
      return true;
    }
  } else { // lhs < 0
    auto minRhs = std::numeric_limits<IntegralT>::min() - lhs;
    if (rhs < minRhs) {
      return true;
    }
  }
  return false;
}

template <typename IntegralT,
    typename std::enable_if<std::is_integral_v<IntegralT> && std::is_unsigned_v<IntegralT>, int>::type = 0>
bool willAddOverflowOrUnderflow(IntegralT lhs, IntegralT rhs) {
  auto maxRhs = std::numeric_limits<IntegralT>::max() - lhs;
  return rhs > maxRhs;
}

template <typename IntegralT,
    typename std::enable_if<std::is_integral_v<IntegralT> && std::is_signed_v<IntegralT>, int>::type = 0>
bool willMultiplyOverflowOrUnderflow(IntegralT lhs, IntegralT rhs) {
  if (lhs == 0 || rhs == 0) {
    return false;
  }

  if (lhs == -1 && rhs == std::numeric_limits<IntegralT>::min()) {
    return true;
  }
  if (rhs == -1 && lhs == std::numeric_limits<IntegralT>::min()) {
    return true;
  }

  if ((lhs > 0 && rhs > 0) || (lhs < 0 && rhs < 0)) {
    if (lhs < 0) {
      lhs = -lhs;
      rhs = -rhs;
    }
    return rhs > std::numeric_limits<IntegralT>::max() / lhs;
  } else {
    if (lhs > 0) {
      // Make sure lhs is negative and rhs is positive.
      std::swap(lhs, rhs);
    }
    return rhs > std::numeric_limits<IntegralT>::min() / lhs;
  }
}

template <typename IntegralT,
    typename std::enable_if<std::is_integral_v<IntegralT> && std::is_unsigned_v<IntegralT>, int>::type = 0>
bool willMultiplyOverflowOrUnderflow(IntegralT lhs, IntegralT rhs) {
  if (lhs == 0 || rhs == 0) {
    return false;
  }
  return rhs > std::numeric_limits<IntegralT>::max() / lhs;
}

template <typename T>
bool tryAppendIntegralDigit(T& value, int base, int d, bool& fit) {
  if (!fit) {
    return false;
  }

  if (willMultiplyOverflowOrUnderflow<T>(value, base)) {
    fit = false;
    return false;
  }
  T tmp = value * base;
  if (willAddOverflowOrUnderflow<T>(tmp, d)) {
    fit = false;
    return false;
  }

  value = tmp + d;
  return true;
}

} // namespace <anonymous>

void Lexer::lexNumberLiteral(SourceLocation startLoc, std::optional<char> sign) {
  // Regular expression for identifying number literals:
  //  [+-]?(0|0x|0X)?[0-9a-fA-F]+((\.?[0-9a-fA-F]+)([eE][+-]?\d+)?)?[lLfF]?

  auto negative = sign.has_value() && sign.value() == '-';

  auto prefix = NumberLiteralPrefix::None;
  auto ch = ensurePeekChar();
  if (ch == '0') {
    if (peekChar(ch)) {
      if (ch == 'x' || ch == 'X') {
        consumeChar();
        prefix = NumberLiteralPrefix::Hex;
      } else if (isOct(ch)) {
        consumeChar();
        prefix = NumberLiteralPrefix::Oct;
      }
    }
  }

  int64_t i64Value = 0;
  double fpValue = 0;
  auto i64Fit = true;
  auto isInteger = true;

  const int base = getBase(prefix);

  while (peekChar(ch) && isDigitUnderPrefix(ch, prefix)) {
    consumeChar();
    auto d = parseHex(ch);

    // value = value * base + d
    tryAppendIntegralDigit(i64Value, base, d, i64Fit);
    fpValue = fpValue * base + d;
  }

  if (peekChar(ch) && (ch == '.' || ch == 'e' || ch == 'E')) {
    i64Fit = false;
    isInteger = false;

    if (ch == '.') {
      consumeChar();

      double fractionalScale = 1.0 / base;
      while (peekChar(ch) && isDigitUnderPrefix(ch, prefix)) {
        consumeChar();
        auto d = parseHex(ch);

        fpValue += d * fractionalScale;
        fractionalScale /= base;
      }
    }
  }

  if (peekChar(ch) && (ch == 'e' || ch == 'E')) {
    consumeChar();
    auto exponentSign = false;
    auto exponent = 0;
    auto exponentFit = true;

    if (peekChar(ch) && (ch == '+' || ch == '-')) {
      consumeChar();
      exponentSign = (ch == '-');
    }

    while (peekChar(ch) && std::isdigit(ch)) {
      consumeChar();
      auto d = parseHex(ch);
      tryAppendIntegralDigit(exponent, 10, d, exponentFit);
    }

    if (exponentFit) {
      if (exponentSign) {
        exponent = -exponent;
      }
      fpValue *= std::pow(10.0, static_cast<double>(exponent));
    } else {
      fpValue = std::numeric_limits<double>::infinity();
    }
  }

  auto suffix = NumberLiteralSuffix::None;
  if (peekChar(ch) && (ch == 'l' || ch == 'L' || ch == 'f' || ch == 'F')) {
    consumeChar();
    if (ch == 'l' || ch == 'L') {
      suffix = NumberLiteralSuffix::Long;
    } else { // ch == 'f' || ch == 'F'
      suffix = NumberLiteralSuffix::Float;
    }
  }

  auto f64Fit = !std::isinf(fpValue);

  if (i64Fit && negative) {
    i64Value = -i64Value;
  }
  if (f64Fit && negative) {
    fpValue = -fpValue;
  }

  auto endLoc = GetNextLocation();
  SourceRange range { startLoc, endLoc };

  if (suffix == NumberLiteralSuffix::Long && !i64Fit) {
    auto diagMsg = DiagnosticsMessage::CreateLiteral(DiagnosticsLevel::Error, range,
        "Number literal cannot fit into 64-bit integer type.");
    _ci.GetDiagnosticsEngine().Emit(*diagMsg);
  }

  if (suffix == NumberLiteralSuffix::Float && !f64Fit) {
    auto diagMsg = DiagnosticsMessage::CreateLiteral(DiagnosticsLevel::Error, range,
        "Number literal cannot fit into double precision floating point type.");
    _ci.GetDiagnosticsEngine().Emit(*diagMsg);
  }

  if (suffix == NumberLiteralSuffix::None && !f64Fit && !i64Fit) {
    auto diagMsg = DiagnosticsMessage::CreateLiteral(DiagnosticsLevel::Error, range,
        "Number literal cannot fit into either 64-bit integer type or double precision floating point type.");
    _ci.GetDiagnosticsEngine().Emit(*diagMsg);
  }

  if (suffix == NumberLiteralSuffix::None && !i64Fit && isInteger) {
    auto diagMsg = DiagnosticsMessage::CreateLiteral(DiagnosticsLevel::Warning, range,
        "Number literal is written in integer form but cannot fit in 64-bit integer type. "
        "Fallback to interpret it as a double precision floating point value instead.");
    _ci.GetDiagnosticsEngine().Emit(*diagMsg);
  }

  switch (suffix) {
    case NumberLiteralSuffix::Long:
      _peekBuffer = std::make_unique<NumberLiteralToken>(i64Value, prefix, suffix, range);
      break;

    case NumberLiteralSuffix::Float:
      _peekBuffer = std::make_unique<NumberLiteralToken>(fpValue, prefix, suffix, range);
      break;

    default: // must be NumberLiteralSuffix::None
      if (i64Fit) {
        _peekBuffer = std::make_unique<NumberLiteralToken>(i64Value, prefix, suffix, range);
      } else {
        _peekBuffer = std::make_unique<NumberLiteralToken>(fpValue, prefix, suffix, range);
      }
  }
}

namespace {

class UnknownDelimiterDiagnosticsMessage : public DiagnosticsMessage {
public:
  explicit UnknownDelimiterDiagnosticsMessage(char ch, SourceLocation loc)
    : DiagnosticsMessage { DiagnosticsLevel::Error, loc },
      _ch(ch)
  { }

  void DumpMessage(StreamWriter &output) const override {
    output << "Unknown delimiter: `" << _ch << '`';
  }

private:
  char _ch;
};

} // namespace <anonymous>

void Lexer::lexDelimiter(SourceLocation startLoc) {
  auto ch = ensureReadChar();
  auto endLoc = GetNextLocation();
  SourceRange range { startLoc, endLoc };

  DelimiterKind kind;
  switch (ch) {
    case '{':
      kind = DelimiterKind::OpenCurlyBrase;
      break;

    case '}':
      kind = DelimiterKind::CloseCurlyBrase;
      break;

    case '[':
      kind = DelimiterKind::OpenBracketBrase;
      break;

    case ']':
      kind = DelimiterKind::CloseBracketBrase;
      break;

    case '(':
      kind = DelimiterKind::OpenParen;
      break;

    case ')':
      kind = DelimiterKind::CloseParen;
      break;

    case ',':
      kind = DelimiterKind::Comma;
      break;

    case '.':
      kind = DelimiterKind::Dot;
      break;

    case ';':
      kind = DelimiterKind::Semicolon;
      break;

    case '@':
      kind = DelimiterKind::At;
      break;

    default: {
      UnknownDelimiterDiagnosticsMessage diagMsg { ch, startLoc };
      _ci.GetDiagnosticsEngine().Emit(diagMsg);
    }
  }

  _peekBuffer = std::make_unique<DelimiterToken>(kind, range);
}

namespace {

class UnknownOperatorDiagnosticsMessage : public DiagnosticsMessage {
public:
  explicit UnknownOperatorDiagnosticsMessage(char ch, SourceLocation loc)
    : DiagnosticsMessage { DiagnosticsLevel::Error, loc },
      _ch(ch)
  { }

  void DumpMessage(StreamWriter &output) const override {
    output << "Unknown operator: `" << _ch << '`';
  }

private:
  char _ch;
};

} // namespace <anonymous>

void Lexer::lexOperator(SourceLocation startLoc) {
  auto ch = ensureReadChar();

  OperatorKind kind;
  switch (ch) {
    case '+':
      if (peekChar(ch) && (ch == '=' || ch == '+')) {
        consumeChar();
        if (ch == '=') {
          // +=
          kind = OperatorKind::AddAssignment;
        } else { // ch == '+'
          // ++
          kind = OperatorKind::Increment;
        }
        break;
      }
      // +
      kind = OperatorKind::Add;
      break;

    case '&':
      if (peekChar(ch) && (ch == '=' || ch == '&')) {
        consumeChar();
        if (ch == '=') {
          // &=
          kind = OperatorKind::AndAssignment;
        } else { // ch == '&'
          // &&
          kind = OperatorKind::LogicalAnd;
        }
        break;
      }
      // &
      kind = OperatorKind::And;
      break;

    case '=':
      if (peekChar(ch) && ch == '=') {
        // ==
        consumeChar();
        kind = OperatorKind::Equal;
      } else {
        // =
        kind = OperatorKind::Assignment;
      }
      break;

    case '~':
      kind = OperatorKind::BitwiseNeg;
      break;

    case '|':
      if (peekChar(ch) && (ch == '=' || ch == '|')) {
        consumeChar();
        if (ch == '=') {
          // |=
          kind = OperatorKind::OrAssignment;
        } else { // ch == '|'
          // ||
          kind = OperatorKind::LogicalOr;
        }
        break;
      }
      // |
      kind = OperatorKind::Or;
      break;

    case '^':
      if (peekChar(ch) && ch == '=') {
        // ^=
        consumeChar();
        kind = OperatorKind::XorAssignment;
        break;
      }
      // ^
      kind = OperatorKind::Xor;
      break;

    case '?':
      kind = OperatorKind::QuationMark;
      break;

    case ':':
      kind = OperatorKind::Colon;
      break;

    case '-':
      if (peekChar(ch) && (ch == '=' || ch == '-')) {
        consumeChar();
        if (ch == '=') {
          // -=
          kind = OperatorKind::SubtractAssignment;
        } else { // ch == '-'
          // --
          kind = OperatorKind::Decrement;
        }
        break;
      }
      // -
      kind = OperatorKind::Subtract;
      break;

    case '/':
      if (peekChar(ch) && ch == '=') {
        // /=
        consumeChar();
        kind = OperatorKind::DivideAssignment;
        break;
      }
      // /
      kind = OperatorKind::Divide;
      break;

    case '>':
      if (peekChar(ch) && (ch == '=' || ch == '>')) {
        consumeChar();
        if (ch == '=') {
          // >=
          kind = OperatorKind::GreaterOrEqual;
          break;
        }
        // ch == '>'
        // >>?

        if (peekChar(ch) && (ch == '=' || ch == '>')) {
          consumeChar();
          if (ch == '=') {
            // >>=
            kind = OperatorKind::RightShiftAssignment;
            break;
          }
          // ch == '>'
          // >>>?

          if (peekChar(ch) && (ch == '=')) {
            // >>>=
            consumeChar();
            kind = OperatorKind::UnsignedRightShiftAssignment;
            break;
          } else {
            // >>>
            kind = OperatorKind::UnsignedRightShift;
            break;
          }
        } else {
          // >>
          kind = OperatorKind::RightShift;
          break;
        }
      }
      kind = OperatorKind::Greater;
      break;

    case '<':
      if (peekChar(ch) && (ch == '<' || ch == '=')) {
        consumeChar();
        if (ch == '=') {
          // <=
          kind = OperatorKind::LessOrEqual;
          break;
        }
        // ch == '<'
        // <<?

        if (peekChar(ch) && ch == '=') {
          // <<=
          consumeChar();
          kind = OperatorKind::LeftShiftAssignment;
          break;
        } else {
          // <<
          kind = OperatorKind::LeftShift;
          break;
        }
      }
      // <
      kind = OperatorKind::Less;
      break;

    case '%':
      if (peekChar(ch) && ch == '=') {
        // %=
        consumeChar();
        kind = OperatorKind::ModuloAssignment;
      } else {
        // %
        kind = OperatorKind::Modulo;
      }
      break;

    case '*':
      if (peekChar(ch) && ch == '=') {
        // *=
        consumeChar();
        kind = OperatorKind::MultiplyAssignment;
      } else {
        // *
        kind = OperatorKind::Multiply;
      }
      break;

    case '!':
      if (peekChar(ch) && ch == '=') {
        // !=
        consumeChar();
        kind = OperatorKind::NotEqual;
      } else {
        // !
        kind = OperatorKind::Not;
      }
      break;

    default: {
      UnknownOperatorDiagnosticsMessage diagMsg { ch, startLoc };
      _ci.GetDiagnosticsEngine().Emit(diagMsg);
    }
  }

  auto endLoc = GetNextLocation();
  SourceRange range { startLoc, endLoc };
  _peekBuffer = std::make_unique<OperatorToken>(kind, range);
}

void Lexer::lexDivideOperatorOrComment(SourceLocation startLoc) {
  auto ch = ensureReadChar();
  assert(ch == '/' && "next character is not as expected to be the start of a divide operator or a comment.");

  if (peekChar(ch)) {
    if (ch == '/' || ch == '*') {
      // //? /*?
      lexComment(startLoc);
      return;
    }

    if (ch == '=') {
      // /=
      consumeChar();
      auto endLoc = GetNextLocation();
      SourceRange range { startLoc, endLoc };
      _peekBuffer = std::make_unique<OperatorToken>(OperatorKind::DivideAssignment, range);
      return;
    }
  }

  // /
  auto endLoc = GetNextLocation();
  SourceRange range { startLoc, endLoc };
  _peekBuffer = std::make_unique<OperatorToken>(OperatorKind::Divide, range);
}

void Lexer::lexComment(SourceLocation startLoc) {
  // Notice that the initial state of the underlying reader should be something like the following figure:
  //  // blah blah blah
  //   ^-- at here
  //  /* blah blah blah */
  //   ^-- at here
  // a.k.a. the leading slash character has already been consumed.
  auto ch = ensureReadChar();
  assert((ch == '/' || ch == '*') && "next character is not as expected to be the start of a comment.");

  if (ch == '/') {
    lexLineComment(startLoc);
  } else { // ch == '*'
    lexBlockComment(startLoc);
  }
}

void Lexer::lexBlockComment(SourceLocation startLoc) {
  std::string content;

  char ch;
  while (true) {
    ch = ensureReadChar();
    if (ch == '*') {
      char nextChar;
      if (peekChar(nextChar) && nextChar == '/') {
        consumeChar();
        break;
      }
    }
    content.push_back(ch);
  }

  auto endLoc = GetNextLocation();
  SourceRange range { startLoc, endLoc };
  _peekBuffer = std::make_unique<CommentToken>(std::move(content), CommentKind::BlockComment, range);
}

void Lexer::lexLineComment(SourceLocation startLoc) {
  std::string content;

  char ch;
  while (peekChar(ch) && ch != '\n') {
    consumeChar();
    content.push_back(ch);
  }

  auto endLoc = GetNextLocation();
  SourceRange range { startLoc, endLoc };
  _peekBuffer = std::make_unique<CommentToken>(std::move(content), CommentKind::LineComment, range);
}

void Lexer::lexWhitespace(SourceLocation startLoc) {
  auto ch = ensureReadChar();
  assert(std::isspace(ch) && "next character is not as expected to be the start of a whitespace token.");

  while (peekChar(ch) && std::isspace(ch)) {
    consumeChar();
  }

  auto endLoc = GetNextLocation();
  SourceRange range { startLoc, endLoc };
  _peekBuffer = std::make_unique<WhitespaceToken>(range);
}

} // namespace jvc
