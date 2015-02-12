#include "builder.h"
#include "keywords.h"
#include "parserutil.h"
#include "group.h"
#include "regex.h"
#include "expander.h"

#include <algorithm>
#include <stdexcept>
#include <functional>

namespace RegexBuilder {
//-----------------------------------------------------------------------------------------------
    Builder::Builder()
        : lastClass_()
        , inlineCounter_(0)
        , regexClasses_()
    {

    }
//-----------------------------------------------------------------------------------------------
    void Builder::compile(std::vector<RegexBuilder::Token> &tokens)
    {
        auto findStart =
        [](auto current, auto last)
        {
            return std::find_if(current, last, [](Token const& tok) {
                 return tok.type == TokenTypes::SECTION_START;
            });
        };

        auto findEnd =
        [](auto current, auto last)
        {
            for (; current != last; ++current)
            {
                if ((current+1) != last)
                {
                    if (current->type == TokenTypes::LINE_FEED && (current+1)->type == TokenTypes::SECTION_END)
                        return current + 1;
                    else if ((current-1)->type == TokenTypes::SEMICOLON && current->type == TokenTypes::SECTION_END && (current+1)->type == TokenTypes::LINE_FEED)
                        return current;
                }
            }
            return last;
        };

        //////////////////////////////////////////////////////////////////////////////
        /// START PARSING
        //////////////////////////////////////////////////////////////////////////////

        auto lastToken = std::end(tokens);
        for (auto currentToken = std::begin(tokens); currentToken != lastToken; ++currentToken)
        {
            using namespace std::literals::string_literals;

            if (currentToken->type == TokenTypes::LINE_FEED)
                continue;

            auto nextToken = currentToken + 1;

            if (isKeyword(currentToken->data))
                currentToken->type = TokenTypes::KEYWORD;

            if (currentToken->type == TokenTypes::KEYWORD)
            {
                decltype(currentToken) sectionEnd;
                decltype(currentToken) sectionStart;

                std::string className;

                auto&& keyword = currentToken->data;
                if ((   keyword == "regex"
                    ||  keyword == "group"
                    ||  keyword == "set"))
                {
                    sectionStart = findStart(currentToken, lastToken);
                    sectionEnd = findEnd(sectionStart, lastToken);

                    if (sectionStart >= sectionEnd || sectionStart == lastToken || sectionEnd == lastToken)
                        throw std::runtime_error ("format is invalid");

                    if (sectionStart == nextToken)
                        throw std::runtime_error ("class does not have a name");

                    if (nextToken->type != TokenTypes::IDENTIFIER)
                        throw std::runtime_error ("class name must be identifier");

                    className = "~" + nextToken->data;
                    lastClass_ = className;
                }

                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                auto extractAttributes = [&]() -> std::string {
                    auto colonToken = nextToken + 1; // misnomer
                    if (colonToken != lastToken && colonToken->type == TokenTypes::COLON)
                    {
                        std::string attribute = "";
                        auto attributeToken = colonToken + 1;
                        if (attributeToken != lastToken && attributeToken->type == TokenTypes::IDENTIFIER)
                        {
                            attribute = attributeToken->data;
                        }
                        else
                            throw std::runtime_error ("class qualifier ist not an identifier");

                        auto curlyToken = attributeToken + 1;
                        if (curlyToken == lastToken || curlyToken->type != TokenTypes::SECTION_START)
                        {
                            if (curlyToken != lastToken)
                                throw std::runtime_error ("open curly expected after class attribute, got: "s + curlyToken->data);
                            else
                                throw std::runtime_error ("open curly expected after class attribute, got the end of input");
                        }

                        return attribute;
                    }
                    else
                    {
                        auto curlyToken = nextToken + 1;
                        if (curlyToken == lastToken || curlyToken->type != TokenTypes::SECTION_START)
                        {
                            if (curlyToken != lastToken)
                                throw std::runtime_error ("open curly or attribute expected after class name, got: "s + curlyToken->data);
                            else
                                throw std::runtime_error ("open curly or attribute expected after class name, got the end of input");
                        }
                    }
                    return "";
                };
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                // remove ~
                className = className.substr(1, className.length() - 1);
                auto attribute = extractAttributes();

                std::vector <std::string> attrList = {};
                if (!attribute.empty())
                    attrList.push_back(attribute);
                createClass(keyword, className, attrList, currentToken + 3 + 2*attrList.size(), sectionEnd);

                currentToken = sectionEnd;
            }
        }
    }
//-----------------------------------------------------------------------------------------------
    std::string Builder::execute(const std::string &mainClass)
    {
        if (regexClasses_.find(std::string("~") + mainClass) == std::end(regexClasses_))
            throw std::runtime_error ((std::string("there is no regex called ") + mainClass + ". I don't know where to start then :(").c_str());

        auto&& main = regexClasses_[std::string("~") + mainClass];

        return Expander{&this->regexClasses_}.expand(*main.get());
    }
//-----------------------------------------------------------------------------------------------
    std::string Builder::getLastClass() const
    {
        return lastClass_;
    }
//-----------------------------------------------------------------------------------------------
    std::string Builder::createInlineClass(std::string const& type, std::vector <std::string> const& attributeList, std::vector<Token>::iterator tokensBegin, std::vector<Token>::iterator tokensEnd)
    {
        ++inlineCounter_;
        using namespace std::literals::string_literals;
        std::string name = "__inline_"s + std::to_string(inlineCounter_);

        std::vector<Token> inlineClassTokens;
        inlineClassTokens.insert(inlineClassTokens.end(), tokensBegin, tokensEnd);
        inlineClassTokens.push_back( {"\n", TokenTypes::LINE_FEED} );
        inlineClassTokens.push_back( {"}", TokenTypes::SECTION_END} );

        for (auto& i : inlineClassTokens)
        {
            if (i.data == "__INLINE_CR__")
            {
                i.data = "\n";
                i.type = TokenTypes::LINE_FEED;
            }
        }

        createClass(type, name, attributeList, std::begin(inlineClassTokens), std::end(inlineClassTokens));

        return name;
    }
//-----------------------------------------------------------------------------------------------
    void Builder::createClass(std::string const& type, std::string const& name, std::vector<std::string> const& attributeList,
                              std::vector<Token>::iterator tokensBegin, std::vector<Token>::iterator tokensEnd)
    {
        using namespace std::literals::string_literals;
        auto t = classNameToType(type);
        switch (t)
        {
            case (ClassTypes::REGEX):
            {
                regexClasses_["~"s + name].reset(new Regex(this, tokensBegin, tokensEnd));
                break;
            }
            case (ClassTypes::GROUP):
            {
                regexClasses_["~"s + name].reset(new Group(this, tokensBegin, tokensEnd));
                break;
            }
            case (ClassTypes::REGEX_SET):
            {
                regexClasses_["~"s + name].reset(new Set(this, tokensBegin, tokensEnd));
                break;
            }
        }
        for (auto const& i : attributeList)
            if (!regexClasses_["~"s + name]->parseAttribute(i)) throw std::runtime_error("unknown attribute in inline class");
    }
//-----------------------------------------------------------------------------------------------
}
