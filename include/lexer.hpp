#pragma once

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
        LessThan,
        LessEquals,
        GreaterThan,
        GreaterEquals,
        Equals,
        Comma,
        OpenParen,
        CloseParen,
        Identifier,
        EndOfFile,
        Error,
    };

    using enum Type;

    constexpr explicit Token(Type type, It lexeme_start, It lexeme_end)
        : type(type), lexeme_start(lexeme_start), lexeme_end(lexeme_end) {}

    template <class Out>
    friend Out& operator<<(Out& out, const Token<It>& token) {
        auto it = token.lexeme_start;
        for (; it != token.lexeme_end; ++it) {
            out << *it;
        }
        return out;
    }

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
constexpr auto consume_while(It& begin, It end, Predicate predicate) -> It& {
    for (; begin != end and predicate(begin); ++begin) {
    }
    return begin;
}

template <class It>
constexpr auto consume_whitespaces(It& begin, It end) {
    return consume_while(begin, end, is_whitespace<const It&>) == end;
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

    if (*begin == 'e') {
        consume_while(++begin, end, is_numeric<const It&>);
        if (*begin == '-') {
            consume_while(++begin, end, is_numeric<const It&>);
        }
    }

    return Token{Token<It>::Number, prev, begin};
}

template <class It>
constexpr auto lex_alpha(It& begin, It end) -> Token<It> {
    return Token{Token<It>::Identifier, begin,
                 consume_while(begin, end, is_alphabetic<const It&>)};
}

template <class It>
constexpr auto lex_operator(It& begin, It end) -> Token<It> {
    auto prev = begin;

    switch (*begin) {
        case '+':
            return Token{Token<It>::Add, prev, ++begin};
        case '-':
            return Token{Token<It>::Sub, prev, ++begin};
        case '*':
            ++begin;
            if (begin != end and *begin == '*') {
                return Token{Token<It>::Pow, prev, ++begin};
            }
            return Token{Token<It>::Mul, prev, begin};
        case '/':
            return Token{Token<It>::Div, prev, ++begin};
        case '%':
            return Token{Token<It>::Mod, prev, ++begin};
        case '^':
            return Token{Token<It>::Pow, prev, ++begin};
        case ',':
            return Token{Token<It>::Comma, prev, ++begin};
        case '(':
            return Token{Token<It>::OpenParen, prev, ++begin};
        case ')':
            return Token{Token<It>::CloseParen, prev, ++begin};
        case '<':
            ++begin;
            if (begin != end and *begin == '=') {
                return Token{Token<It>::LessEquals, prev, ++begin};
            }
            return Token{Token<It>::LessThan, prev, begin};
        case '>':
            ++begin;
            if (begin != end and *begin == '=') {
                return Token{Token<It>::GreaterEquals, prev, ++begin};
            }
            return Token{Token<It>::GreaterThan, prev, begin};
        case '=':
            ++begin;
            if (begin != end and *begin == '=') {
                return Token{Token<It>::Equals, prev, ++begin};
            }
            // return Token{Token<It>::Assign, prev, begin};
            return Token{Token<It>::Error, prev, begin};
        default:
            return Token{Token<It>::Error, prev, begin};
    }
}

}  // namespace detail

template <class It>
constexpr auto next_token(It& begin, It end) -> Token<It> {
    using namespace detail;

    if (begin == end or consume_whitespaces(begin, end)) {
        return Token{Token<It>::EndOfFile, begin, end};
    }

    const auto token = lex_operator(begin, end);
    if (token.lexeme_start != token.lexeme_end) {
        return token;
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
