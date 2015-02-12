#include "parserutil.h"

#include <regex>

namespace RegexBuilder {
    boost::optional<std::string> validateRegexSyntax(std::string const& regexString)
    {
        try {
            std::regex _(regexString);
            return {};
        } catch (std::regex_error const& exc) {
            using namespace std::regex_constants;
            using namespace std::literals::string_literals;

            switch(exc.code()) {
                case(error_collate): return{"The expression contained an invalid collating element name."s};
                case(error_ctype):	return{"The expression contained an invalid character class name."s};
                case(error_escape):	return{"The expression contained an invalid escaped character, or a trailing escape."s};
                case(error_backref):	return{"The expression contained an invalid back reference."s};
                case(error_brack):	return{"The expression contained mismatched brackets ([ and ])."s};
                case(error_paren):	return{"The expression contained mismatched parentheses (( and ))."s};
                case(error_brace):	return{"The expression contained mismatched braces ({ and })."s};
                case(error_badbrace):	return{"The expression contained an invalid range between braces ({ and })."s};
                case(error_range):	return{"The expression contained an invalid character range."s};
                case(error_space):	return{"There was insufficient memory to convert the expression into a finite state machine."s};
                case(error_badrepeat):	return{"The expression contained a repeat specifier (one of *?+{) that was not preceded by a valid regular expression."s};
                case(error_complexity):	return{"The complexity of an attempted match against a regular expression exceeded a pre-set level."s};
                case(error_stack):	return{"There was insufficient memory to determine whether the regular expression could match the specified character sequence."s};
            }
            return {"unknown regex error"s};
        }
    }

    std::string tokenListToString(const std::vector<Token> &tokens)
    {
        std::string result = "";
        for (auto const& i : tokens)
            result += i.data;
        return result;
    }
} // namespace RegexBuilder
