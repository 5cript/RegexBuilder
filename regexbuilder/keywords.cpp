#include "keywords.h"

#include <algorithm>

namespace RegexBuilder {
    bool isKeyword(const std::string &toCheck)
    {
        for (auto const& i : keywords)
            if (toCheck == i)
                return true;
        return false;
    }
} // namespace RegexBuilder
