#ifndef PARSERUTIL_H
#define PARSERUTIL_H

#include "token.h"
#include "keywords.h"

#include <string>
#include <boost/optional.hpp>

namespace RegexBuilder {
    template <typename IteratorType>
    bool convertToKeyword(IteratorType& current, IteratorType next, IteratorType last)
    {
        // convert IDENTIFIER to KEYWORD
        if (    current->type == TokenTypes::IDENTIFIER
            &&  isKeyword(current->data)
            &&  next != last
            &&  (next->type == TokenTypes::COLON || next->type == TokenTypes::SECTION_START))
        {
            current->type = TokenTypes::KEYWORD;
            return true;
        }
        return false;
    }

    template <typename IteratorType>
    std::vector <Token> accumulateTokens(IteratorType begin, IteratorType end) {
        auto result = std::vector<Token>{};
        for (; begin != end && begin->type != TokenTypes::LINE_FEED; ++begin)
            result.push_back(*begin);
        return result;
    }

    std::string tokenListToString(std::vector<Token> const& tokens);

    boost::optional<std::string> validateRegexSyntax(std::string const& regexString);
}

#endif // PARSERUTIL_H

