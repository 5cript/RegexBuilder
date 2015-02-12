#ifndef PARSER
#define PARSER

#include "regexclass.h"
#include "token.h"
#include "builder_fwd.h"

#include <map>
#include <vector>
#include <string>
#include <memory>

namespace RegexBuilder {
    class Builder {
    public:
        Builder();

        /**
         * @brief compile Parses the token list and transforms it into classes.
         * @param tokens A list of tokens.
         */
        void compile(std::vector <Token>& tokens);

        /**
         * @brief execute Start generation of regex and start with mainClass.
         * @param mainClass The class to start generation from.
         * @return
         */
        std::string execute(std::string const& mainClass); // requires a main regex

        /**
         * @brief getLastClass Returns the class name that got processed last.
         * @return  Returns the class name that got processed last.
         */
        std::string getLastClass() const;

        /**
         * @brief createInlineClass Creates a new class from the tokens und generates a name for it.
         * @param type Can be one of: group, regex, set, etc. throws otherwise.
         * @param tokensBegin The begin of a token range to pass to the ctor.
         * @param tokensEnd The end of a token range to pass to the ctor.
         * @return The newly generated name (something along of __inline_1, __inline_2, ...)
         */
        std::string createInlineClass(std::string const& type, std::vector <std::string> const& attributeList,
                                      std::vector<Token>::iterator tokensBegin, std::vector<Token>::iterator tokensEnd);

        /**
         * @brief createClass Creates a new class from the tokens.
         * @param type The type of the class
         * @param name The name of the class
         * @param attributeList The attributes of the class
         * @param tokensBegin The begin of a token range to pass to the ctor.
         * @param tokensEnd The end of a token range to pass to the ctor.
         */
        void createClass(std::string const& type, std::string const& name, std::vector <std::string> const& attributeList,
                         std::vector<Token>::iterator tokensBegin, std::vector<Token>::iterator tokensEnd);

    private:
        std::string lastClass_;
        int inlineCounter_;
        std::map <std::string, std::unique_ptr <RegexClass>> regexClasses_;
    };
}

#endif // PARSER

