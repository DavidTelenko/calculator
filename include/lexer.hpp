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
        Identifier,
        EndOfFile,
        Error,
    };

    constexpr auto get_name() {
        switch (type) {
            case Type::Number:
                return "Number";
            case Type::Plus:
                return "Plus";
            case Type::Minus:
                return "Minus";
            case Type::Star:
                return "Star";
            case Type::Slash:
                return "Slash";
            case Type::Caret:
                return "Caret";
            case Type::OpenParen:
                return "OpenParen";
            case Type::CloseParen:
                return "CloseParen";
            case Type::Identifier:
                return "Identifier";
            case Type::EndOfFile:
                return "EndOfFile";
            case Type::Error:
                return "Error";
        }
    }

    constexpr explicit Token(Type type, It lexeme_start, It lexeme_end)
        : type(type), lexeme_start(lexeme_start), lexeme_end(lexeme_end) {}

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
    auto it = begin;

    for (; it != end and is_numeric(it); ++it) {
    }

    if (is_dot(it)) {
        for (++it; it != end and is_numeric(it); ++it) {
        }
    }

    return Token<It>{Token<It>::Type::Number, begin, it};
}

template <class It>
constexpr auto lex_alpha(It begin, It end) -> Token<It> {
    auto it = begin;

    for (; it != end and is_alphabetic(it); ++it) {
    }

    return Token<It>{Token<It>::Type::Identifier, begin, it};
}

template <class It>
constexpr auto next_token(It begin, It end) -> Token<It> {
    if (begin == end) {
        return Token<It>{Token<It>::Type::EndOfFile, begin, end};
    }

    for (; is_whitespace(begin); ++begin) {
        if (begin == end) {
            return Token<It>{Token<It>::Type::EndOfFile, begin, end};
        }
    }

    switch (*begin) {
        case '+':
            return Token{Token<It>::Type::Plus, begin, ++begin};
        case '-':
            return Token{Token<It>::Type::Minus, begin, ++begin};
        case '*':
            return Token{Token<It>::Type::Star, begin, ++begin};
        case '/':
            return Token{Token<It>::Type::Slash, begin, ++begin};
        case '^':
            return Token{Token<It>::Type::Caret, begin, ++begin};
        case '(':
            return Token{Token<It>::Type::OpenParen, begin, ++begin};
        case ')':
            return Token{Token<It>::Type::CloseParen, begin, ++begin};
        default:
            break;
    }

    if (is_numeric(begin)) {
        return lex_number(begin, end);
    }

    if (is_alphabetic(begin)) {
        return lex_alpha(begin, end);
    }

    return Token(Token<It>::Type::Error, begin, end);
}

template <class It, class Out>
constexpr auto lex_all(It begin, It end, Out out) {
    for (;; ++out) {
        const auto token = calc::next_token(begin, end);

        if (token.type == calc::Token<const char*>::Type::EndOfFile or
            token.type == calc::Token<const char*>::Type::Error) {
            break;
        }

        begin = token.lexeme_end;
        *out = token;
    }

    return out;
}

}  // namespace calc
