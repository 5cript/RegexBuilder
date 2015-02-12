#include "regex.h"
#include "parserutil.h"

#include <stdexcept>

namespace RegexBuilder {
    Regex::Regex(Builder* context, std::vector<Token>::iterator sectionStart, std::vector<Token>::iterator sectionEnd)
        : context_(context)
    {
        parse(context, sectionStart, sectionEnd);
    }

    void Regex::setMember(const std::string&, const std::vector<Token> &)
    {
        throw std::runtime_error ("unexpected identifier before colon");
    }

    bool Regex::parseAttribute(const std::string &)
    {
        return false;
    }
}
