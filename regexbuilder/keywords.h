#ifndef KEYWORDS_H
#define KEYWORDS_H

#include <string>

namespace RegexBuilder {
    constexpr const char* keywords[] = {
        "regex",
        "set",
        "group",
        "from",
        "to",
        "front",
        "glue",
        "back",
        "contains",
        "atomic",
        "capture",
        "raw",
        "insensitive"
    };

    bool isKeyword(std::string const& toCheck);
} // namespace RegexBuilder

#endif // KEYWORDS_H

