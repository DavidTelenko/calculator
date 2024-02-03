#pragma once

namespace calc {

template <class It>
struct Token {
    enum class Type {
        Number,
        Plus,
        Minus,
        Star,
        Slash,
        Caret,
        OpenParen,
        CloseParen,
        EndOfFile,
        Error,
    };
    Type type;
    It lexeme_start, lexeme_end;
};

template <class It>
constexpr auto is_whitespace(It&& iterator) -> bool {
    return *iterator == ' ' or *iterator == '\t' or *iterator == '\n';
}

template <class It>
constexpr auto is_alphabetic(It&& iterator) -> bool {
    return ('a' <= *iterator and *iterator <= 'z') or
           ('A' <= *iterator and *iterator <= 'Z');
}

template <class It>
constexpr auto is_numeric(It&& iterator) -> bool {
    return ('0' <= *iterator and *iterator <= '9');
}

template <class It>
constexpr auto is_dot(It&& iterator) -> bool {
    return *iterator == '.';
}

template <class It>
constexpr auto lex_number(It begin, It end) -> Token<It> {
    It it = begin;

    for (; it != end and is_numeric(it); ++it) {
    }

    if (is_dot(it)) {
        for (++it; it != end and is_numeric(it); ++it) {
        }
    }

    return Token<It>{
        .type = Token<It>::Type::Number,
        .lexeme_start = begin,
        .lexeme_end = it,
    };
}

template <class It>
constexpr auto next_token(It& begin, const It end) -> Token<It> {
    for (; is_whitespace(begin); ++begin) {
        if (begin == end) {
            return Token<It>{
                .type = Token<It>::Type::EndOfFile,
                .lexeme_start = begin,
                .lexeme_end = end,
            };
        }
    }

    switch (*(++begin)) {
        case '+':
            return Token{Token<It>::Type::Plus, begin, begin++};
        case '-':
            return Token{Token<It>::Type::Minus, begin, begin++};
        case '*':
            return Token{Token<It>::Type::Star, begin, begin++};
        case '/':
            return Token{Token<It>::Type::Slash, begin, begin++};
        case '^':
            return Token{Token<It>::Type::Caret, begin, begin++};
        case '(':
            return Token{Token<It>::Type::OpenParen, begin, begin++};
        case '(':
            return Token{Token<It>::Type::CloseParen, begin, begin++};
        default:
            break;
    }

    if (is_numeric(begin)) {
        return lex_number(begin, end);
    }

    return Token(Token<It>::Type::Error, begin, end);
}

}  // namespace calc
