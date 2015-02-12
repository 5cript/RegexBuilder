#ifndef EXPANDER_H
#define EXPANDER_H

#include "token.h"
#include "regexclass.h"
#include "regex.h"
#include "group.h"
#include "set.h"

#include <map>
#include <string>
#include <memory>
#include <vector>
#include <deque>
#include <algorithm>

namespace RegexBuilder {

    constexpr unsigned int recursionDepthLimit = 5000;

    class Expander {
    private: // captures
        std::map <std::string, std::unique_ptr <RegexClass>>* regexClasses_;

    public:
        std::map <int, std::string> operator()(Regex const& rgx, std::deque <int>& expandableParts);
        std::map <int, std::string> operator()(Group const& group, std::deque <int>& expandableParts);
        std::map <int, std::string> operator()(Set const& set, std::deque <int>& expandableParts);

        template <typename RegexClassType>
        std::map <int, std::string> defaultExpand(RegexClassType const& expandable, std::deque <int>& expandableParts);

        std::string expand(RegexClass const& expandable);
        std::map <int, std::string> expandOnce(RegexClass const& expandable, std::deque <int>& expandableParts);

        Expander(decltype(regexClasses_) regexClasses);
    };

    template <typename RegexClassType>
    std::map <int, std::string> Expander::defaultExpand(const RegexClassType &expandable, std::deque <int>& expandableParts)
    {
        std::map <int, std::string> regexParts;
        for (auto i : expandable.contains)
        {
            regexParts[i.first] = i.second;
            expandableParts.push_back(i.first);
        }
        for (auto const& i : expandable.raw)
        {
            regexParts[i.first] = i.second;
        }
        for (auto const& i : expandable.insensitive)
        {
            regexParts[i.first] = i.second;
        }

        return regexParts;
    }

}

#endif // EXPANDER_H

