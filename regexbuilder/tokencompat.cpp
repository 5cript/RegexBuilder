#include "tokencompat.h"

std::vector<RegexBuilder::Token> convertTokens(std::vector <WaveToken> preproTokens)
{
    using namespace RegexBuilder;

    std::vector <Token> result;
    for (auto const& token : preproTokens)
    {
        TokenTypes tokType = TokenTypes::OTHER;
        if (token.data == ":")
            tokType = TokenTypes::COLON;
        else if (token.data == "{")
            tokType = TokenTypes::SECTION_START;
        else if (token.data == "}")
            tokType = TokenTypes::SECTION_END;
        else if (token.id == boost::wave::T_STRINGLIT)
            tokType = TokenTypes::STRING_LITERAL;
        else if (token.data == ";")
            tokType = TokenTypes::SEMICOLON;
        else if (token.id == boost::wave::T_NEWLINE)
            tokType = TokenTypes::LINE_FEED;
        else if (CATEGORY_FROM_TOKEN(token.id) == CATEGORY_FROM_TOKEN(boost::wave::IdentifierTokenType))
            tokType = TokenTypes::IDENTIFIER;
        else if (CATEGORY_FROM_TOKEN(token.id) == CATEGORY_FROM_TOKEN(boost::wave::KeywordTokenType))
            tokType = TokenTypes::IDENTIFIER;
        else {
            // ignore all whitespace:
            using namespace boost::wave;

            switch (token.id) {
                case(T_CCOMMENT): continue;
                case(T_CPPCOMMENT): continue;
                case(T_SPACE): continue;
                case(T_SPACE2): continue;
                case(T_EOF): continue;
                case(T_CONTLINE): continue;
                default: break;
            }
        }

        result.push_back(Token{token.data, tokType});
    }
    return result;
}
