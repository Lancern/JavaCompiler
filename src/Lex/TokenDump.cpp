//
// Created by Sirui Mu on 2019/12/23.
//

#include "Lex/Token.h"
#include "Infrastructure/Stream.h"

namespace jvc {

namespace {

const char* KeywordNames[] = {
#define DEF_KEYWORD_NAME(kw) #kw,
  JVC_KEYWORD_LIST(DEF_KEYWORD_NAME)
#undef DEF_KEYWORD_NAME
};

} // namespace jvc

void KeywordToken::Dump(StreamWriter& o) const {
  o << "Keyword `" << KeywordNames[static_cast<int>(_keywordKind)] << "` (";
  range().Dump(o);
  o << ")";
}

void IdentifierToken::Dump(StreamWriter &o) const {
  o << "Identifier `" << _name << "` (";
  range().Dump(o);
  o << ")";
}

void NumberLiteralToken::Dump(StreamWriter &o) const {
  o << "NumberLiteral ";
  if (IsInteger()) {
    o << _intValue << ' ';
  } else {
    o << "<non-integer> ";
  }
  o << _floatValue << " (";
  range().Dump(o);
  o << ")";
}

void StringLiteralToken::Dump(StreamWriter &o) const {
  o << "StringLiteral `" << _content << "` (";
  range().Dump(o);
  o << ")";
}


void CharacterLiteralToken::Dump(StreamWriter &o) const {
  o << "CharacterLiteral `" << _ch << "` (";
  range().Dump(o);
  o << ")";
}

namespace {

const char* DelimiterNames[] = {
#define DEF_DELIMITER_NAME(v) #v,
  JVC_DELIMITER_LIST(DEF_DELIMITER_NAME)
#undef DEF_DELIMITER_NAME
};

} // namespace <anonymous>

void DelimiterToken::Dump(StreamWriter &o) const {
  o << "Delimiter <" << DelimiterNames[static_cast<int>(_kind)] << "> (";
  range().Dump(o);
  o << ")";
}

namespace {

const char* OperatorNames[] = {
#define DEF_OPERATOR_NAME(v) #v,
  JVC_OPERATOR_LIST(DEF_OPERATOR_NAME)
#undef DEF_OPERATOR_NAME
};

} // namespace <anonymous>

void OperatorToken::Dump(StreamWriter &o) const {
  o << "Operator <" << OperatorNames[static_cast<int>(_kind)] << "> (";
  range().Dump(o);
  o << ")";
}

void CommentToken::Dump(StreamWriter &o) const {
  o << "Comment ";
  switch (_kind) {
    case CommentKind::LineComment:
      o << "<LineComment> ";
      break;

    case CommentKind::BlockComment:
      o << "<BlockComment> ";
      break;

    default:
      o << "<UnknownCommentType> ";
      break;
  }

  o << "`" << _content << "` (";
  range().Dump(o);
  o << ")";
}

void WhitespaceToken::Dump(StreamWriter &o) const {
  o << "Whitespace (";
  range().Dump(o);
  o << ")";
}

} // namespace jvc
