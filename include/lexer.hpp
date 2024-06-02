#pragma once

#include <iostream>
namespace calc {

template <class It>
struct Token {
    enum class Type {
        Number,
        Add,
        Sub,
        Mul,
        Div,
        Mod,
        Pow,
        Comma,
        OpenParen,
        CloseParen,
        Identifier,
        EndOfFile,
        Error,
    };

    template <class Out>
    friend Out& operator<<(Out& out, const Token<It>& token) {
        auto it = token.lexeme_start;
        for (; it != token.lexeme_end; ++it) {
            out << *it;
        }
        return out;
    }

    using enum Type;

    constexpr explicit Token(Type type, It lexeme_start, It lexeme_end)
        : type(type), lexeme_start(lexeme_start), lexeme_end(lexeme_end) {}

    Type type;
    It lexeme_start, lexeme_end;
};

namespace detail {

template <class It>
constexpr auto is_whitespace(It&& iterator) -> bool {
    return *iterator == ' ' or *iterator == '\t' or *iterator == '\n';
}

template <class It>
constexpr auto is_alphabetic(It&& iterator) -> bool {
    return ('a' <= *iterator and *iterator <= 'z') or
           ('A' <= *iterator and *iterator <= 'Z') or *iterator == '_';
}

template <class It>
constexpr auto is_numeric(It&& iterator) -> bool {
    return ('0' <= *iterator and *iterator <= '9');
}

template <class It>
constexpr auto is_hex(It&& iterator) -> bool {
    return ('0' <= *iterator and *iterator <= '9') or
           ('a' <= *iterator and *iterator <= 'f') or
           ('A' <= *iterator and *iterator <= 'F');
}

template <class It>
constexpr auto is_oct(It&& iterator) -> bool {
    return ('0' <= *iterator and *iterator <= '7');
}

template <class It>
constexpr auto is_bin(It&& iterator) -> bool {
    return ('0' <= *iterator and *iterator <= '1');
}

template <class It, class Predicate>
constexpr auto consume_while(It& begin, It end, Predicate predicate) {
    for (; begin != end and predicate(begin); ++begin) {
    }
}

template <class It>
constexpr auto lex_number(It& begin, It end) -> Token<It> {
    const auto prev = begin;

    if (*begin == '0') {
        ++begin;

        // hex, oct, bin cases
        if (*begin == 'x') {
            ++begin;
            consume_while(begin, end, is_hex<const It&>);
            return Token{Token<It>::Number, prev, begin};
        } else if (*begin == 'o') {
            ++begin;
            consume_while(begin, end, is_oct<const It&>);
            return Token{Token<It>::Number, prev, begin};
        } else if (*begin == 'b') {
            ++begin;
            consume_while(begin, end, is_bin<const It&>);
            return Token{Token<It>::Number, prev, begin};
        }
    }

    consume_while(begin, end, is_numeric<const It&>);

    // float case
    if (*begin == '.') {
        consume_while(++begin, end, is_numeric<const It&>);
    }

    return Token{Token<It>::Number, prev, begin};
}

template <class It>
constexpr auto lex_alpha(It& begin, It end) -> Token<It> {
    auto prev = begin;

    consume_while(begin, end, is_alphabetic<const It&>);

    return Token{Token<It>::Identifier, prev, begin};
}

}  // namespace detail

template <class It>
constexpr auto next_token(It& begin, It end) -> Token<It> {
    using namespace detail;

    if (begin == end) {
        return Token{Token<It>::EndOfFile, begin, end};
    }

    for (; is_whitespace(begin); ++begin) {
        if (begin == end) {
            return Token{Token<It>::EndOfFile, begin, end};
        }
    }

    switch (*begin) {
        case '+':
            return Token{Token<It>::Add, begin, ++begin};
        case '-':
            return Token{Token<It>::Sub, begin, ++begin};
        case '*':
            return Token{Token<It>::Mul, begin, ++begin};
        case '/':
            return Token{Token<It>::Div, begin, ++begin};
        case '%':
            return Token{Token<It>::Mod, begin, ++begin};
        case '^':
            return Token{Token<It>::Pow, begin, ++begin};
        case ',':
            return Token{Token<It>::Comma, begin, ++begin};
        case '(':
            return Token{Token<It>::OpenParen, begin, ++begin};
        case ')':
            return Token{Token<It>::CloseParen, begin, ++begin};
        default:
            break;
    }

    if (is_numeric(begin)) {
        return lex_number(begin, end);
    }

    if (is_alphabetic(begin)) {
        return lex_alpha(begin, end);
    }

    return Token(Token<It>::Error, begin, end);
}

template <class It>
constexpr auto peek_token(It begin, It end) {
    return next_token(begin, end);
}

template <class It, class Consumer>
constexpr auto lex_all(It begin, It end, Consumer out) {
    using namespace detail;

    for (; begin != end;) {
        const auto token = next_token(begin, end);

        if (not out(token) or token.type == Token<It>::EndOfFile) {
            break;
        }
    }

    return out;
}

}  // namespace calc
