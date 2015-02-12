#include "keywords.h"

#include <algorithm>

namespace RegexBuilder {
    bool isKeyword(const std::string &toCheck)
    {
        return std::end(keywords) == std::find(std::begin(keywords), std::end(keywords), toCheck.c_str());
    }
} // namespace RegexBuilder
