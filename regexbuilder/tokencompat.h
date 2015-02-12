#ifndef TOKENCOMPAT_H
#define TOKENCOMPAT_H

#include "regexbuilder/token.h"
#include "regexbuilder/wave.h"

#include <vector>
#include <string>
#include <memory>

struct WaveToken
{
    std::string data;
    boost::wave::token_id id;

    WaveToken(std::string data, boost::wave::token_id id) : data(std::move(data)), id(id) {}
};

std::vector <RegexBuilder::Token> convertTokens(std::vector <WaveToken> preproTokens);

#endif // TOKENCOMPAT_H
