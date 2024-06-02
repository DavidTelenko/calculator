#pragma once

#include <charconv>
#include <cmath>
#include <deque>
#include <lexer.hpp>
#include <vector>

#include "prelude.hpp"

namespace calc {

namespace detail {  // namespace detail

template <class It>
constexpr auto precedence(const Token<It>& token) {
    switch (token.type) {
        case Token<It>::OpenParen:
        case Token<It>::CloseParen:
            return 0;
        case Token<It>::Add:
        case Token<It>::Sub:
            return 1;
        case Token<It>::Mul:
        case Token<It>::Div:
        case Token<It>::Mod:
            return 2;
        case Token<It>::Pow:
            return 3;
        case Token<It>::Number:
        case Token<It>::Comma:
        case Token<It>::Identifier:
        case Token<It>::EndOfFile:
        case Token<It>::Error:
            return 4;
    }
}

template <class It>
constexpr auto is_function(const Token<It>& token, It begin, It end) -> bool {
    if (token.type != Token<It>::Identifier) {
        return false;
    }

    const auto next = next_token(begin, end).type;

    return next == Token<It>::OpenParen or next == Token<It>::Identifier or
           next == Token<It>::Number;
}

template <class It>
constexpr auto is_binary_operator(const Token<It>& token) -> bool {
    return token.type >= Token<It>::Add and token.type <= Token<It>::Pow;
}

// template <class It>

}  // namespace detail

template <class It>
constexpr auto parse(It begin, It end) -> std::vector<Token<It>> {
    using namespace detail;

    std::vector<Token<It>> output;
    std::vector<Token<It>> operators;

    for (;;) {
        const auto token = next_token(begin, end);

        if (token.type == Token<It>::EndOfFile or
            token.type == Token<It>::Error) {
            break;
        }

        else if (is_function(token, begin, end) or
                 token.type == Token<It>::OpenParen) {
            operators.push_back(token);
        }

        else if (token.type == Token<It>::Number or
                 token.type == Token<It>::Identifier) {
            output.push_back(token);
        }

        else if (is_binary_operator(token)) {
            while (not operators.empty() and
                   precedence(operators.back()) >= precedence(token)) {
                output.push_back(operators.back());
                operators.pop_back();
            }
            operators.push_back(token);
        }

        else if (token.type == Token<It>::Comma) {
            constexpr auto err =
                "Mismatched parenthesis or function argument separators";

            ASSERT(not operators.empty(), err);

            while (operators.back().type != Token<It>::OpenParen) {
                output.push_back(operators.back());
                operators.pop_back();

                ASSERT(not operators.empty(), err);
            }
            continue;
        }

        else if (token.type == Token<It>::CloseParen) {
            constexpr auto err = "Mismatched parenthesis";

            ASSERT(not operators.empty(), err);

            while (operators.back().type != Token<It>::OpenParen) {
                output.push_back(operators.back());
                operators.pop_back();

                ASSERT(not operators.empty(), err);
            }

            operators.pop_back();

            if (not operators.empty() and
                is_function(operators.back(), begin, end)) {
                output.push_back(operators.back());
                operators.pop_back();
            }
        }
    }

    while (operators.size()) {
        constexpr auto err = "Mismatched parenthesis";

        ASSERT(operators.back().type != Token<It>::OpenParen, err);

        output.push_back(operators.back());
        operators.pop_back();
    }

    return output;
}

}  // namespace calc
