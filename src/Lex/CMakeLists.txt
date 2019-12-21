add_library(JVCLex STATIC
        Lexer.cpp
        LexerStreamReader.cpp
        LexerStreamReader.h
        ${JVC_INCLUDE_DIR}/Lex/Lexer.h
        ${JVC_INCLUDE_DIR}/Lex/Token.h)
target_link_libraries(JVCLex JVCFrontend JVCInfrastructure)
