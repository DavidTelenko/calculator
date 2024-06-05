#pragma once

#include <cmath>
#include <iterator>
#include <numeric>
#include <string_view>

#include "lexer.hpp"
#include "parser.hpp"
#include "prelude.hpp"

namespace calc {

namespace detail {

template <class F, class It, class Stack, class Vars>
constexpr auto perform(const Token<It>& identifier, Stack& stack, Vars vars)
    -> std::optional<F> {
    using val = std::iter_value_t<It>;
    using str_view = typename std::basic_string_view<val>;

    const str_view identifier_name{identifier.lexeme_start,
                                   identifier.lexeme_end};

    const auto variable = vars(identifier_name);

    if (variable) {
        return variable;
    }

    ASSERT(not stack.empty(),
           "Identifier `" << identifier_name << "` not found");

    const F a = stack.back();
    stack.pop_back();

    if (identifier_name == "sqrt") {
        return std::sqrt(a);
    } else if (identifier_name == "cbrt") {
        return std::cbrt(a);
    }

    else if (identifier_name == "abs") {
        return std::fabs(a);
    }

    else if (identifier_name == "ln") {
        return std::log(a);
    } else if (identifier_name == "lg") {
        return std::log10(a);
    } else if (identifier_name == "exp") {
        return std::exp(a);
    }

    else if (identifier_name == "ceil") {
        return std::ceil(a);
    } else if (identifier_name == "floor") {
        return std::floor(a);
    } else if (identifier_name == "round") {
        return std::round(a);
    } else if (identifier_name == "trunc") {
        return std::trunc(a);
    }

    else if (identifier_name == "sin") {
        return std::sin(a);
    } else if (identifier_name == "asin") {
        return std::asin(a);
    } else if (identifier_name == "sinh") {
        return std::sinh(a);
    } else if (identifier_name == "asinh") {
        return std::asinh(a);
    }

    else if (identifier_name == "cos") {
        return std::cos(a);
    } else if (identifier_name == "acos") {
        return std::acos(a);
    } else if (identifier_name == "cosh") {
        return std::cosh(a);
    } else if (identifier_name == "acosh") {
        return std::acosh(a);
    }

    else if (identifier_name == "tan") {
        return std::tan(a);
    } else if (identifier_name == "atan") {
        return std::atan(a);
    } else if (identifier_name == "tanh") {
        return std::tanh(a);
    } else if (identifier_name == "atanh") {
        return std::atanh(a);
    }

    else if (identifier_name == "ctan") {
        return std::tan(calc::HALF_PI - a);
    } else if (identifier_name == "actan") {
        return std::atan(calc::HALF_PI - a);
    } else if (identifier_name == "ctanh") {
        return std::tanh(calc::HALF_PI - a);
    } else if (identifier_name == "actanh") {
        return std::atanh(calc::HALF_PI - a);
    }

    ASSERT(not stack.empty(),
           "Identifier `" << identifier_name << "` not found");

    const F b = stack.back();
    stack.pop_back();

    switch (identifier.type) {
        case Token<It>::Add:
            return (b + a);
        case Token<It>::Sub:
            return (b - a);
        case Token<It>::Mul:
            return (b * a);
        case Token<It>::Div:
            return (b / a);
        case Token<It>::Mod:
            return std::fmod(b, a);
        case Token<It>::Pow:
            return std::pow(b, a);
        case Token<It>::LessThan:
            return b < a;
        case Token<It>::LessEquals:
            return b <= a;
        case Token<It>::GreaterThan:
            return b > a;
        case Token<It>::GreaterEquals:
            return b >= a;
        case Token<It>::Equals:
            return std::abs(b - a) <= 1e-40;
        default:
            break;
    }

    if (identifier_name == "min") {
        return std::min(b, a);
    } else if (identifier_name == "max") {
        return std::max(b, a);
    } else if (identifier_name == "log") {
        return std::log(a) / std::log(b);
    } else if (identifier_name == "gcd") {
        return std::gcd(static_cast<int>(b), static_cast<int>(a));
    } else if (identifier_name == "lcm") {
        return std::lcm(static_cast<int>(b), static_cast<int>(a));
    } else if (identifier_name == "or") {
        return static_cast<F>(static_cast<size_t>(b) or static_cast<size_t>(a));
    } else if (identifier_name == "and") {
        return static_cast<F>(static_cast<size_t>(b) and
                              static_cast<size_t>(a));
    } else if (identifier_name == "xor") {
        return static_cast<F>(static_cast<size_t>(b) xor
                              static_cast<size_t>(a));
    } else if (identifier_name == "add") {
        return b + a;
    } else if (identifier_name == "sub") {
        return b - a;
    } else if (identifier_name == "div") {
        return b / a;
    } else if (identifier_name == "mul") {
        return b * a;
    } else if (identifier_name == "mod") {
        return std::fmod(b, a);
    } else if (identifier_name == "pow") {
        return std::pow(b, a);
    }

    ASSERT(false, "Identifier `" << identifier_name << "` not found");

    return std::nullopt;
}

template <class F, class It>
auto parse_num(It it) -> F {
    if (*it->lexeme_start == '0') {
        size_t val;
        const auto next = it->lexeme_start + 1;

        switch (*next) {
            case 'x':
                std::from_chars(it->lexeme_start + 2, it->lexeme_end, val, 16);
                return static_cast<F>(val);
            case 'o':
                std::from_chars(it->lexeme_start + 2, it->lexeme_end, val, 8);
                return static_cast<F>(val);
            case 'b':
                std::from_chars(it->lexeme_start + 2, it->lexeme_end, val, 2);
                return static_cast<F>(val);
        }
    }
    F val;
    std::from_chars(it->lexeme_start, it->lexeme_end, val);
    return val;
}

}  // namespace detail

template <class T, class It>
concept iterator_mapper =
    requires(T variables, std::basic_string_view<std::iter_value_t<It>> str) {
        std::floating_point<decltype(variables(str))>;
    };

template <class F, class It>
constexpr auto evaluate(It begin, It end, iterator_mapper<It> auto variables)
    -> std::optional<F> {
    using namespace detail;

    const auto queue = parse(begin, end);

    if (not queue) {
        return std::nullopt;
    }

    // calc::print([](auto&& v) { std::cout << v; }, *queue)('\n');

    std::vector<F> stack;

    for (auto it = queue->begin(); it != queue->end();) {
        if (it->type == Token<It>::Number) {
            stack.push_back(parse_num<F>(it));
            ++it;
            continue;
        }

        const auto result = perform<F, It>(*it, stack, variables);

        if (not result) {
            return result;
        }

        stack.push_back(*result);
        ++it;
    }

    ASSERT(stack.size() == 1, "Reduntant values");

    return stack.back();
}

}  // namespace calc
