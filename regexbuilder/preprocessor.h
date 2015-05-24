#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "wave.h"

#include <vector>
#include <string>
#include <unordered_map>

namespace RegexBuilder {

    const std::string mainClassMacroName = "MAIN_REGEX_CLASS";
    const std::string onlyPreprocessFlag = "PREPROCESS_ONLY";

    template <typename InputIterator>
    std::vector <WaveToken> preprocess(InputIterator begin,
                                       InputIterator end,
                                       std::string const& libraryPath,
                                       std::vector <std::string> const& searchDirectories,
                                       std::vector <boost::wave::preprocess_exception>& messages,
                                       std::unordered_map <std::string, std::vector <std::string>>& specialDefinitions,
                                       std::string& preprocessedOuptut)
    {
        std::vector <WaveToken> tokens;

        typedef boost::wave::cpplexer::lex_iterator<
                boost::wave::cpplexer::lex_token<> >
                lex_iterator_type;
        typedef boost::wave::context<
                std::string::iterator, lex_iterator_type>
                context_type;

        context_type ctx(begin, end, "OPENED_FILE");
        ctx.set_language(boost::wave::support_cpp11);

        ctx.add_sysinclude_path(libraryPath.c_str());

        for (auto const& i : searchDirectories)
            ctx.add_include_path(i.c_str());

        context_type::iterator_type first = ctx.begin();
        context_type::iterator_type last = ctx.end();

        preprocessedOuptut = "";

        bool finished = false;
        do {
            try
            {
                for (;first != last; ++first)
                {
                    auto id = (boost::wave::token_id)(*first);
                    if (id == boost::wave::T_PP_LINE)
                    {
                        for (; first != last && ![&first](){return (boost::wave::token_id)(*first) == boost::wave::T_NEWLINE;}(); ++first);
                        continue;
                    }

                    if (first->get_value() == "__INSERT_CR__")
                    {
                        tokens.emplace_back (
                            std::string(""),
                            boost::wave::T_NEWLINE
                        );
                        preprocessedOuptut += "\n";
                    }
                    else
                    {
                        tokens.emplace_back (
                            std::string(first->get_value().c_str()),
                            id
                        );
                        preprocessedOuptut += std::string(first->get_value().c_str());
                    }
                }
                finished = true;
            }
            catch (boost::wave::preprocess_exception& exc)
            {
                // ui->regexOutput->setPlainText(QString(exc.description()) + " in line " + QString::number(exc.line_no()) + " in file " + QString(exc.file_name()));
                messages.push_back(exc);
                if (exc.is_recoverable())
                    finished = false;
                else
                    throw std::exception();
            }
        } while (!finished);

        {
            bool is_function_style;
            bool is_predefined;
            context_type::position_type pos;
            std::vector<context_type::token_type> parameters;
            context_type::token_sequence_type definition;

            if (ctx.get_macro_definition(mainClassMacroName, is_function_style, is_predefined, pos, parameters, definition))
            {
                if (!definition.empty())
                {
                    std::vector <std::string> tokenStrings;
                    for (auto const& token : definition)
                        tokenStrings.push_back(token.get_value().c_str());
                    specialDefinitions[mainClassMacroName] = tokenStrings;
                }
            }

            if (ctx.get_macro_definition(onlyPreprocessFlag, is_function_style, is_predefined, pos, parameters, definition))
            {
                specialDefinitions[onlyPreprocessFlag] = {};
            }
        }
        return tokens;
    }

}

#endif // PREPROCESSOR_H

