add_library(JVCLex STATIC
        Lexer.cpp
        LexerStreamReader.cpp
        LexerStreamReader.h
        ${JVC_INCLUDE_DIR}/Infrastructure/Stream.h
        ${JVC_INCLUDE_DIR}/Frontend/SourceLocation.h
        ${JVC_INCLUDE_DIR}/Lex/Lexer.h
        ${JVC_INCLUDE_DIR}/Lex/Token.h)
