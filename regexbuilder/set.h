#ifndef SET
#define SET

#include "token.h"
#include "regexclass.h"

#include <string>
#include <vector>

namespace RegexBuilder{

    class Set : public RegexClass
    {
    public:
        Set(Builder* context,
            std::vector <Token>::iterator sectionStart,
            std::vector <Token>::iterator sectionEnd,
            bool negate = false);

        void setMember(std::string const& memberName, std::vector <Token> const& data);
        bool parseAttribute(std::string const& attr);

        bool isNegated() const;

    private:
        Builder* context_;
        bool negate_;
    };

} // namespace RegexBuilder

#endif // SET

