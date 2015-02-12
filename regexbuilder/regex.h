#ifndef REGEX_H
#define REGEX_H

#include "token.h"
#include "regexclass.h"

#include <vector>

namespace RegexBuilder {
    class Regex : public RegexClass {
    public:
        Regex() = default;
        Regex(Builder* context,
              std::vector <Token>::iterator sectionStart,
              std::vector <Token>::iterator sectionEnd);

        void setMember(std::string const& memberName, std::vector <Token> const& data);
        bool parseAttribute(std::string const& str);

    private:
        Builder* context_;
    };
} // namespace RegexBuilder

#endif // REGEX_H

