#include "set.h"

namespace RegexBuilder {

    Set::Set(Builder* context, std::vector<Token>::iterator sectionStart, std::vector<Token>::iterator sectionEnd, bool negate)
        : context_(context)
        , negate_(negate)
    {
        parse(context_, sectionStart, sectionEnd);
    }

    void Set::setMember(const std::string&, const std::vector<Token> &)
    {
        throw std::runtime_error ("unexpected identifier before colon");
    }

    bool Set::parseAttribute(const std::string &attr)
    {
        if (attr == "negate")
            negate_ = true;
        else
            return false;
        return true;
    }

    bool Set::isNegated() const
    {
        return negate_;
    }

} // namespace RegexBuilder
