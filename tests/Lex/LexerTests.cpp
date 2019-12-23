//
// Created by Sirui Mu on 2019/12/22.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include "gtest/gtest.h"

#include "Infrastructure/Stream.h"
#include "Frontend/CompilerInstance.h"
#include "Lex/Token.h"
#include "Lex/Lexer.h"

class LexerTest : public ::testing::Test {
protected:
  std::unique_ptr<jvc::Lexer> CreateLexer(const std::string& sourceName, const std::string& source,
                                          jvc::LexerOptions options = jvc::LexerOptions { }) {
    auto stream = jvc::InputStream::FromBuffer(source.data(), source.size());
    ci.GetSourceManager().Load(sourceName, std::move(stream));

    return jvc::Lexer::Create(ci, 1, options);
  }

  jvc::CompilerInstance ci;
};

#define ASSERT_IS_KEYWORD(token, kwKind) \
    ASSERT_TRUE(token) << "token is nullptr"; \
    ASSERT_TRUE(token->IsKeyword()) << "token is not a keyword token"; \
    ASSERT_EQ(dynamic_cast<jvc::KeywordToken *>(token)->keywordKind(), (kwKind)) \
        << "Keyword kind is not correct"

TEST_F(LexerTest, LexKeyword) {
  jvc::LexerOptions options { };
  options.KeepWhitespace = false;
  auto lexer = CreateLexer("name", "public abstract", options);

  auto token = lexer->ReadNextToken();
  ASSERT_IS_KEYWORD(token.get(), jvc::KeywordKind::Public);

  token = lexer->ReadNextToken();
  ASSERT_IS_KEYWORD(token.get(), jvc::KeywordKind::Abstract);

  token = lexer->ReadNextToken();
  ASSERT_FALSE(token) << "Lexer does not return nullptr at EOF.";
}

#define ASSERT_IS_IDENTIFIER(token, identifierName) \
    ASSERT_TRUE(token) << "token is nullptr"; \
    ASSERT_TRUE(token->IsIdentifier()) << "token is not an identifier token"; \
    ASSERT_EQ(dynamic_cast<jvc::IdentifierToken *>(token)->name(), (identifierName)) \
        << "Name of identifier is not correct."

TEST_F(LexerTest, LexIdentifier) {
  jvc::LexerOptions options { };
  options.KeepWhitespace = false;
  auto lexer = CreateLexer("name", "public identifier", options);

  auto token = lexer->ReadNextToken();
  ASSERT_IS_KEYWORD(token.get(), jvc::KeywordKind::Public);

  token = lexer->ReadNextToken();
  ASSERT_IS_IDENTIFIER(token.get(), "identifier");

  token = lexer->ReadNextToken();
  ASSERT_FALSE(token) << "Lexer does not return nullptr at EOF.";
}

#define ASSERT_IS_STRING_LITERAL(token, value) \
    ASSERT_TRUE(token) << "token is nullptr"; \
    ASSERT_TRUE(token->IsLiteral()) << "token is not a literal token"; \
    ASSERT_TRUE(dynamic_cast<jvc::LiteralToken *>(token)->IsString()) << "literal token is not a string literal"; \
    ASSERT_EQ(dynamic_cast<jvc::StringLiteralToken *>(token)->content(), (value)) \
        << "content of string literal is not correct."

TEST_F(LexerTest, LexStringLiteral) {
  jvc::LexerOptions options { };
  options.KeepWhitespace = false;
  auto lexer = CreateLexer("name", "\"literal\\n\\t\\uac12\\123value\" interface", options);

  auto token = lexer->ReadNextToken();
  ASSERT_IS_STRING_LITERAL(token.get(), "literal\n\t\x12\xAC\x53value");

  token = lexer->ReadNextToken();
  ASSERT_IS_KEYWORD(token.get(), jvc::KeywordKind::Interface);

  token = lexer->ReadNextToken();
  ASSERT_FALSE(token) << "lexer does not return nullptr at EOF.";
}

#define ASSERT_IS_INTEGER_LITERAL(token, literalValue, literalPrefix, literalSuffix) \
    ASSERT_TRUE(token) << "token is nullptr"; \
    ASSERT_TRUE(token->IsLiteral()) << "token is not a literal token"; \
    ASSERT_TRUE(dynamic_cast<jvc::LiteralToken *>(token)->IsNumber()) \
        << "literal token is not a number literal"; \
    ASSERT_TRUE(dynamic_cast<jvc::NumberLiteralToken *>(token)->IsInteger()) \
        << "number token is not an integer literal"; \
    ASSERT_EQ(dynamic_cast<jvc::NumberLiteralToken *>(token)->AsInt64(), (literalValue)) \
        << "integer value is not correct"; \
    ASSERT_EQ(dynamic_cast<jvc::NumberLiteralToken *>(token)->prefix(), (literalPrefix)) \
        << "number literal prefix is not correct";\
    ASSERT_EQ(dynamic_cast<jvc::NumberLiteralToken *>(token)->suffix(), (literalSuffix)) \
        << "number literal suffix is not correct"

#define ASSERT_IS_FLOAT_LITERAL(token, literalValue, literalPrefix, literalSuffix) \
    ASSERT_TRUE(token) << "token is nullptr"; \
    ASSERT_TRUE(token->IsLiteral()) << "token is not a literal token"; \
    ASSERT_TRUE(dynamic_cast<jvc::LiteralToken *>(token)->IsNumber()) \
        << "literal token is not a number literal"; \
    ASSERT_DOUBLE_EQ(dynamic_cast<jvc::NumberLiteralToken *>(token)->AsDouble(), (literalValue)) \
        << "floating point value is not correct"; \
    ASSERT_EQ(dynamic_cast<jvc::NumberLiteralToken *>(token)->prefix(), (literalPrefix)) \
        << "number literal prefix is not correct";\
    ASSERT_EQ(dynamic_cast<jvc::NumberLiteralToken *>(token)->suffix(), (literalSuffix)) \
        << "number literal suffix is not correct"

TEST_F(LexerTest, LexNumberLiteral) {
  jvc::LexerOptions options { };
  options.KeepWhitespace = false;
  auto lexer = CreateLexer("name", "-12.14e-2 +014 13e+4 12l 16e-2F", options);

  auto token = lexer->ReadNextToken(); // -12.14e-2
  ASSERT_IS_FLOAT_LITERAL(token.get(), -12.14e-2,
      jvc::NumberLiteralPrefix::None, jvc::NumberLiteralSuffix::None);

  token = lexer->ReadNextToken(); // +014
  ASSERT_IS_INTEGER_LITERAL(token.get(), 12,
      jvc::NumberLiteralPrefix::Oct, jvc::NumberLiteralSuffix::None);

  token = lexer->ReadNextToken(); // 13e+4
  ASSERT_IS_FLOAT_LITERAL(token.get(), 13e+4,
      jvc::NumberLiteralPrefix::None, jvc::NumberLiteralSuffix::None);

  token = lexer->ReadNextToken(); // 12l
  ASSERT_IS_FLOAT_LITERAL(token.get(), 12,
      jvc::NumberLiteralPrefix::None, jvc::NumberLiteralSuffix::Long);

  token = lexer->ReadNextToken(); // 16e-2F
  ASSERT_IS_FLOAT_LITERAL(token.get(), 16e-2,
      jvc::NumberLiteralPrefix::None, jvc::NumberLiteralSuffix::Float);

  token = lexer->ReadNextToken(); // EOF
  ASSERT_FALSE(token) << "lexer does not return nullptr at EOF.";
}

#define ASSERT_IS_DELIMITER(token, delimiterKind) \
    ASSERT_TRUE(token) << "token is nullptr"; \
    ASSERT_TRUE(token->IsDelimiter()) << "token is not a delimiter token"; \
    ASSERT_EQ(dynamic_cast<jvc::DelimiterToken *>(token)->delimiter(), (delimiterKind))

TEST_F(LexerTest, LexDelimiter) {
  jvc::LexerOptions options { };
  options.KeepWhitespace = false;
  auto lexer = CreateLexer("name", "{}.()[] ; @", options);

  auto token = lexer->ReadNextToken();
  ASSERT_IS_DELIMITER(token.get(), jvc::DelimiterKind::OpenCurlyBrase);

  token = lexer->ReadNextToken();
  ASSERT_IS_DELIMITER(token.get(), jvc::DelimiterKind::CloseCurlyBrase);

  token = lexer->ReadNextToken();
  ASSERT_IS_DELIMITER(token.get(), jvc::DelimiterKind::Dot);

  token = lexer->ReadNextToken();
  ASSERT_IS_DELIMITER(token.get(), jvc::DelimiterKind::OpenParen);

  token = lexer->ReadNextToken();
  ASSERT_IS_DELIMITER(token.get(), jvc::DelimiterKind::CloseParen);

  token = lexer->ReadNextToken();
  ASSERT_IS_DELIMITER(token.get(), jvc::DelimiterKind::OpenBracketBrase);

  token = lexer->ReadNextToken();
  ASSERT_IS_DELIMITER(token.get(), jvc::DelimiterKind::CloseBracketBrase);

  token = lexer->ReadNextToken();
  ASSERT_IS_DELIMITER(token.get(), jvc::DelimiterKind::Semicolon);

  token = lexer->ReadNextToken();
  ASSERT_IS_DELIMITER(token.get(), jvc::DelimiterKind::At);

  token = lexer->ReadNextToken();
  ASSERT_FALSE(token) << "Lexer does not return nullptr at EOF.";
}

#define ASSERT_IS_OPERATOR(token, kind) \
    ASSERT_TRUE(token) << "token is nullptr"; \
    ASSERT_TRUE(token->IsOperator()) << "token is not an operator token"; \
    ASSERT_EQ(dynamic_cast<jvc::OperatorToken *>(token)->operatorKind(), (kind)) \
        << "operator kind is not correct."

TEST_F(LexerTest, LexOperator) {
  jvc::LexerOptions options { };
  options.KeepWhitespace = false;
  auto lexer = CreateLexer("name", "+= >>> << <<= !~ / /=", options);

  auto token = lexer->ReadNextToken();
  ASSERT_IS_OPERATOR(token.get(), jvc::OperatorKind::AddAssignment);

  token = lexer->ReadNextToken();
  ASSERT_IS_OPERATOR(token.get(), jvc::OperatorKind::UnsignedRightShift);

  token = lexer->ReadNextToken();
  ASSERT_IS_OPERATOR(token.get(), jvc::OperatorKind::LeftShift);

  token = lexer->ReadNextToken();
  ASSERT_IS_OPERATOR(token.get(), jvc::OperatorKind::LeftShiftAssignment);

  token = lexer->ReadNextToken();
  ASSERT_IS_OPERATOR(token.get(), jvc::OperatorKind::Not);

  token = lexer->ReadNextToken();
  ASSERT_IS_OPERATOR(token.get(), jvc::OperatorKind::BitwiseNeg);

  token = lexer->ReadNextToken();
  ASSERT_IS_OPERATOR(token.get(), jvc::OperatorKind::Divide);

  token = lexer->ReadNextToken();
  ASSERT_IS_OPERATOR(token.get(), jvc::OperatorKind::DivideAssignment);

  token = lexer->ReadNextToken();
  ASSERT_FALSE(token) << "Lexer does not return nullptr at EOF.";
}

#define ASSERT_IS_COMMENT(token, text) \
    ASSERT_TRUE(token) << "token is nullptr"; \
    ASSERT_TRUE(token->IsComment()) << "token is not a comment token"; \
    ASSERT_EQ(dynamic_cast<jvc::CommentToken *>(token)->content(), (text)) \
        << "content of comment is not correct"

TEST_F(LexerTest, LexComment) {
  jvc::LexerOptions options { };
  options.KeepWhitespace = false;
  options.KeepComment = true;
  auto lexer = CreateLexer("name", "/ // public\n /* public\ninterface*/", options);

  auto token = lexer->ReadNextToken();
  ASSERT_IS_OPERATOR(token.get(), jvc::OperatorKind::Divide);

  token = lexer->ReadNextToken();
  ASSERT_IS_COMMENT(token.get(), " public");

  token = lexer->ReadNextToken();
  ASSERT_IS_COMMENT(token.get(), " public\ninterface");

  token = lexer->ReadNextToken();
  ASSERT_FALSE(token) << "Lexer does not return nullptr at EOF.";
}

#pragma clang diagnostic pop