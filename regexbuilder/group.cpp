#include "group.h"
#include "parserutil.h"

#include <stdexcept>

namespace RegexBuilder {

    Group::Group(Builder *context, std::vector<Token>::iterator sectionStart, std::vector<Token>::iterator sectionEnd, GroupType type)
        : context_(context)
        , type_(type)
    {
        parse(context_, sectionStart, sectionEnd);
    }

    void Group::setMember(const std::string&, const std::vector<Token>&)
    {
        throw std::runtime_error ("unexpected identifier before colon");
    }

    bool Group::parseAttribute(const std::string &attr)
    {
        if (attr == "capture")
            type_ = GroupType::CAPTURE;
        else if (attr == "atomic")
            type_ = GroupType::ATOMIC;
        else
            return false;
        return true;
    }

    GroupType Group::getType() const
    {
        return type_;
    }

} // namespace RegexBuilder
