#ifndef REGEXCLASS_H
#define REGEXCLASS_H

#include "token.h"
#include "parserutil.h"
#include "builder_fwd.h"

#include <string>
#include <stdexcept>
#include <vector>
#include <locale>

#include <boost/algorithm/string.hpp>

namespace RegexBuilder
{
    bool sortRegexClasses(std::string const& lhs, std::string const& rhs);

    enum class ClassTypes {
        REGEX,
        GROUP,
        REGEX_SET // for some unexplicable reason does 'SET' not compile???
    };

    ClassTypes classNameToType(std::string const& name);
    std::string classTypeToName(ClassTypes type);


    class RegexClass
    {
    public:
        RegexClass() = default;
        virtual ~RegexClass() = default;

        virtual void setMember(std::string const& memberName, std::vector <Token> const& data) = 0;
        virtual bool parseAttribute(std::string const& attr) = 0;

        void parse(Builder* context, std::vector <Token>::iterator sectionStart, std::vector <Token>::iterator sectionEnd);

        // pair: ORDER, DATA
        std::vector <std::pair <int, std::string>> contains;
        std::vector <std::pair <int, std::string>> raw;
        std::vector <std::pair <int, std::string>> insensitive; // is also raw
    };
}

#endif // REGEXCLASS_H
