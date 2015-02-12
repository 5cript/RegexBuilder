#ifndef TOKES_H
#define TOKES_H

#include <string>

namespace RegexBuilder {
    enum class TokenTypes {
        OTHER,
        RESERVED_1, // used for internals
        RESERVED_2, // used for internals
        RESERVED_3, // used for internals
        RESERVED_4, // used for internals
        IDENTIFIER,
        COLON,
        SECTION_START,
        SECTION_END,
        LINE_FEED,
        KEYWORD,
        SEMICOLON,
        STRING_LITERAL
    };

    struct Token {
        std::string data;
        TokenTypes type;
    };
}

#endif // TOKES_H

