#include "expander.h"
#include "parserutil.h"

#include <type_traits>

namespace RegexBuilder {
//-----------------------------------------------------------------------------------------------
    std::map <int, std::string> Expander::operator()(const Regex &rgx, std::deque <int>& expandableParts)
    {
        return defaultExpand(rgx, expandableParts);
    }
//-----------------------------------------------------------------------------------------------
    std::map <int, std::string> Expander::operator()(const Group &group, std::deque <int>& expandableParts)
    {
        std::string prefix = "(";
        if (group.getType() == GroupType::ATOMIC)
            prefix += "?>";
        else if (group.getType() == GroupType::NON_CAPTURE)
            prefix += "?:";

        auto expansionResult = defaultExpand(group, expandableParts);
        expansionResult[-1] = prefix;
        expansionResult[expansionResult.size() - 1] = ")";
        return expansionResult;
    }
//-----------------------------------------------------------------------------------------------
    std::map <int, std::string> Expander::operator()(const Set &set, std::deque <int>& expandableParts)
    {
        auto prefix = std::string("[");
        if (set.isNegated())
            prefix += "^";

        auto expansionResult = defaultExpand(set, expandableParts);
        expansionResult[-1] = prefix;
        expansionResult[expansionResult.size() - 1] = "]";
        return expansionResult;
    }
//-----------------------------------------------------------------------------------------------
    std::string Expander::expand(const RegexClass &expandable)
    {
        std::deque <int> expandables;
        auto exps = expandOnce(expandable, expandables);

        unsigned int loopBreak = 0;

        auto findBiggestMatch = [this](std::string const& str, std::size_t& pos) {
            std::size_t max = 0;
            std::remove_reference_t<decltype(*regexClasses_)>::iterator biggest = std::end(*regexClasses_);
            for (auto clas = std::begin(*regexClasses_); clas != std::end(*regexClasses_); ++clas)
            {
                auto p = str.find(clas->first);
                if (p != std::string::npos)
                {
                    max = std::max(clas->first.length(), max);
                    biggest = clas;
                    pos = p;
                }
            }
            return biggest;
        };

        // traverse all those that could contain expandable contents:
        while(!expandables.empty())
        {
            ++loopBreak;
            if (loopBreak > recursionDepthLimit)
                throw std::runtime_error("exceeded recursion depth limit (you might have accidentally built a selfreferencing class)");

            auto currentExpansionLine = expandables.front();

            auto str = exps[currentExpansionLine]; // get the string behind the sequence that may contain expandables
            // traverse all possible regex classes defined

            std::size_t pos = std::string::npos;
            auto biggest = findBiggestMatch(str, pos);

            if (biggest != std::end(*regexClasses_) && pos != std::string::npos)
            {
                auto const& clas = *biggest;

                // it does! Now we have to expand that class too. We add the results to our sequence list and
                // update the expandables vector.
                std::string front = str.substr(0, pos);
                std::string back = str.substr(pos + clas.first.length(), str.length() - pos - clas.first.length());

                // expand class:
                std::deque <int> newExpandables;
                auto innerExpandResult = expandOnce(*clas.second, newExpandables);

                auto innerExpansionLineAmount = innerExpandResult.size();

                // move the rest ouf of the way
                for (int j = exps.size() - 1; j != currentExpansionLine; --j)
                {
                    exps[j + 1 /* back */ + innerExpansionLineAmount /* amounts of lines to insert */] = exps[j];
                }

                // update the expandables list
                std::transform(std::begin(expandables), std::end(expandables), std::begin(expandables), [&innerExpansionLineAmount, &currentExpansionLine](int p) -> int
                {
                    // >= if there are more than just one expandable element in this line
                    if (p >= (int)currentExpansionLine) return p + 1 + innerExpansionLineAmount;
                    return p;
                });
                expandables.push_front(currentExpansionLine);

                // modify the inner expansion list to fit
                std::transform(std::begin(newExpandables), std::end(newExpandables), std::begin(newExpandables), [&](int p) -> int
                {
                    return p + currentExpansionLine + 1 - innerExpandResult.begin()->first;
                });

                // insert expansion results:
                exps[currentExpansionLine] = front;
                for (int line = 0; line != (int)innerExpandResult.size(); ++line)
                {
                    exps[currentExpansionLine + line + 1] = innerExpandResult[line + innerExpandResult.begin()->first];
                }
                exps[currentExpansionLine + 1 + innerExpandResult.size()] = back;

                // insert expansion elements:
                expandables.insert(std::begin(expandables) + 1, std::begin(newExpandables), std::end(newExpandables));
            } else
                expandables.pop_front();
        }

        std::string result = "";
        for (auto const& i : exps)
            result += i.second;

        return result;
    }
//-----------------------------------------------------------------------------------------------
    std::map <int, std::string> Expander::expandOnce(const RegexClass &expandable, std::deque <int>& expandableParts)
    {
        if (dynamic_cast <Regex const*>(&expandable)) return operator()(*dynamic_cast <Regex const*>(&expandable), expandableParts);
        if (dynamic_cast <Group const*>(&expandable)) return operator()(*dynamic_cast <Group const*>(&expandable), expandableParts);
        if (dynamic_cast <Set const*>(&expandable)) return operator()(*dynamic_cast <Set const*>(&expandable), expandableParts);

        throw std::runtime_error("not yet implemented class type");
    }
//-----------------------------------------------------------------------------------------------
    Expander::Expander(decltype(regexClasses_) regexClasses)
        : regexClasses_(regexClasses)
    {
    }
//-----------------------------------------------------------------------------------------------
}
