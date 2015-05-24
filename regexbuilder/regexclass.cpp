#include "regexclass.h"
#include "attribute_preprocessor.h"
#include "builder.h"

namespace RegexBuilder {
    bool sortRegexClasses(const std::string &lhs, const std::string &rhs)
    {
        return lhs.length() < rhs.length();
    }
//-----------------------------------------------------------------------------------------------
    ClassTypes classNameToType(const std::string &name)
    {
        using namespace std::literals::string_literals;

        if (name == "regex")
            return ClassTypes::REGEX;
        else if (name == "group")
            return ClassTypes::GROUP;
        else if (name == "set")
            return ClassTypes::REGEX_SET;
        else
            throw std::runtime_error("unknown class name: "s + name);
    }
//-----------------------------------------------------------------------------------------------
    std::string classTypeToName(RegexBuilder::ClassTypes type)
    {
        switch (type)
        {
            case(ClassTypes::REGEX):
                return "regex";
            case(ClassTypes::GROUP):
                return "group";
            case(ClassTypes::REGEX_SET):
                return "set";
            default:
                throw std::runtime_error("unknown class type [id]");
        }
    }
//-----------------------------------------------------------------------------------------------
    void RegexClass::parse(Builder* context, std::vector<Token>::iterator sectionStart, std::vector<Token>::iterator sectionEnd)
    {
        if (sectionStart == sectionEnd)
            throw std::runtime_error ("unexpected end of stream");

        for (auto iter = sectionStart + 1; iter < sectionEnd; ++iter)
        {
            if (iter->type == TokenTypes::LINE_FEED)
                continue;

            auto next = iter + 1;

            if (convertToKeyword(iter, next, sectionEnd))
            {
                // next is valid if we end up here!
                if (next->data != ":") // is no colon? no good!
                    throw std::runtime_error("colon expected");
                else
                {
                    std::vector<Token> attrTokens;
                    attrTokens = accumulateTokens(next + 1, sectionEnd);

                    auto iter2 = next + 1 + attrTokens.size();

                    if (iter2 == sectionEnd)
                        throw std::runtime_error("unexpected end of section on attribute token accumulation");

                    if ((iter2 - 1)->data != ";")
                        throw std::runtime_error ("attribute definition must be ended with semicolon");

                    attrTokens.pop_back();

                    if (iter->data == "contains")
                    {
                        // tokens in a contains statement first undergo a preprocessing line:
                        attrTokens = preprocessAttribute(std::begin(attrTokens), std::end(attrTokens), *context);

                        // concatenate tokens and convert them to a string:
                        auto temp = tokenListToString(attrTokens);

                        // add concatenated tokens to contains list.
                        contains.push_back({contains.size() + raw.size() + insensitive.size(), temp});
                    }
                    else if (iter->data == "raw")
                    {
                        // concatenate tokens to string and create new raw attribute:
                        auto temp = tokenListToString(attrTokens);
                        raw.push_back({contains.size() + raw.size() + insensitive.size(), temp});
                    }
                    else if (iter->data == "insensitive")
                    {
                        auto temp = tokenListToString(attrTokens);
                        std::string replacent;
                        auto locale = std::locale{};
                        for (auto const& c : temp)
                        {
                            if (std::isalpha(c, locale))
                            {
                                replacent += "[";
                                replacent += boost::algorithm::to_lower_copy(std::string({c, '\0'}));
                                replacent += boost::algorithm::to_upper_copy(std::string({c, '\0'}));
                                replacent += "]";
                            }
                            else
                                replacent += c;
                        }
                        insensitive.push_back({contains.size() + raw.size() + insensitive.size(), replacent});
                    }
                    else
                        setMember(iter->data, attrTokens);

                    iter = iter2;

                    // that was it!
                    if (iter == sectionEnd)
                        break;
                }
            }
            else if (next != sectionEnd)
                throw std::runtime_error (std::string("unexpected token before colon in class, got: ") + iter->data);
        }
    }

    //-----------------------------------------------------------------------------------------------
}
