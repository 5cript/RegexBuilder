#ifndef GROUP_H
#define GROUP_H

#include "token.h"
#include "regexclass.h"

#include <vector>

namespace RegexBuilder {
    enum class GroupType {
        NON_CAPTURE,
        CAPTURE,
        ATOMIC
    };

    class Group : public RegexClass {
    public:
        Group() = default;

        Group(Builder* context,
              std::vector <Token>::iterator sectionStart,
              std::vector <Token>::iterator sectionEnd,
              GroupType type = GroupType::NON_CAPTURE);

        void setMember(std::string const& memberName, std::vector <Token> const& data);
        bool parseAttribute(std::string const& attr);

        GroupType getType() const;

    private:
        Builder* context_;
        GroupType type_;

    };
} // namespace RegexBuilder

#endif // GROUP_H

