#ifndef ATTRIBUTE_PREPROCESSOR_H
#define ATTRIBUTE_PREPROCESSOR_H

#include "token.h"
#include "builder.h"
#include "parserutil.h"

#include <stdexcept>
#include <string>
#include <vector>
#include <regex>

namespace RegexBuilder {

    namespace Util {
        /**
         *  Checks if tokens from range2 (begin2, end2) are the same as those in range1 (begin1, end1), but the length
         *  does not need to be the same.
         */
        template <typename TokenIteratorType>
        bool parseInlineClassStart (TokenIteratorType& begin, TokenIteratorType end, std::string& className, std::string& attribute)
        {
            std::string str = tokenListToString({begin, end});

            std::regex rgx("\\(([a-zA-Z_][a-zA-Z0-9_]*)(?::(capture|negate|atomic))?\\)__START__");
            std::smatch result;
            std::regex_search(str, result, rgx);

            if (result.size() >= 2)
            {
                className = result[1];
                if (result.size() >= 3)
                    attribute = result[2];
                return true;
            }
            else
                return false;
        }
    }


    /**
     *  Replaces certain parts of the attribute first before passing it back.
     *  __INLINE__(class_type)__START__...__END__ creates a new named class that is invisible to the user.
     *  It enables more flexibility.
     *
     */
    template <typename IteratorType>
    std::vector <Token> preprocessAttribute(IteratorType begin, IteratorType end, Builder& context)
    {
        std::vector <Token> replacement;
        for (; begin < end; ++begin)
        {
            if (begin->data == "__INLINE__")
            {
                std::string type;
                std::string attr;

                if (!Util::parseInlineClassStart(begin, end, type, attr))
                    throw std::runtime_error("__INLINE__ found but syntax is wrong.");

                auto seeker = [&begin, &end](std::string const& str, std::string const& contraStr = "") {
                    int counter = 1;
                    for (auto seeker = begin; seeker != end; ++seeker)
                    {
                        if (seeker->data == str)
                        {
                            --counter;
                            if (counter == 0)
                                return seeker;
                        }
                        if (!contraStr.empty() && seeker->data == contraStr)
                            ++counter;
                    }
                    return end;
                };

                begin = seeker("__START__") + 1;
                auto inlineEnd = seeker("__END__", "__START__");

                if (inlineEnd == end)
                    throw std::runtime_error("__INLINE__ found but __END__ not found");

                std::vector <std::string> attrList;
                if (attr != "")
                    attrList.push_back(attr);

                auto name = context.createInlineClass(type, attrList, begin, inlineEnd /* past the end */);

                replacement.push_back({"~", TokenTypes::OTHER});
                replacement.push_back({name, TokenTypes::IDENTIFIER});

                begin = inlineEnd;
            }
            else
                replacement.push_back(*begin);
        }
        return replacement;
    }

} // namespace RegexBuilder

#endif // ATTRIBUTE_PREPROCESSOR_H

