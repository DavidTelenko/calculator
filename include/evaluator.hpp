#pragma once

#include <cmath>
#include <cstdint>
#include <functional>
#include <iterator>
#include <numeric>
#include <string_view>

#include "lexer.hpp"
#include "parser.hpp"
#include "prelude.hpp"

namespace calc {

namespace detail {

template <class F, class It, class Stack>
constexpr auto perform(const Token<It>& function, Stack& stack) -> F {
    using val = std::iter_value_t<It>;
    using str_view = typename std::basic_string_view<val>;

    constexpr auto half_pi = std::numbers::pi_v<F> / F{2};

    const str_view function_name{function.lexeme_start, function.lexeme_end};

    ASSERT(not stack.empty(),
           "Function `" << function_name << "` call is incorrect");

    const F a = stack.back();
    stack.pop_back();

    if (function_name == "sqrt") {
        return std::sqrt(a);
    } else if (function_name == "cbrt") {
        return std::cbrt(a);
    }

    else if (function_name == "abs") {
        return std::fabs(a);
    }

    else if (function_name == "ln") {
        return std::log(a);
    } else if (function_name == "lg") {
        return std::log10(a);
    } else if (function_name == "exp") {
        return std::exp(a);
    }

    else if (function_name == "ceil") {
        return std::ceil(a);
    } else if (function_name == "floor") {
        return std::floor(a);
    } else if (function_name == "round") {
        return std::round(a);
    } else if (function_name == "trunc") {
        return std::trunc(a);
    }

    else if (function_name == "sin") {
        return std::sin(a);
    } else if (function_name == "asin") {
        return std::asin(a);
    } else if (function_name == "sinh") {
        return std::sinh(a);
    } else if (function_name == "asinh") {
        return std::asinh(a);
    }

    else if (function_name == "cos") {
        return std::cos(a);
    } else if (function_name == "acos") {
        return std::acos(a);
    } else if (function_name == "cosh") {
        return std::cosh(a);
    } else if (function_name == "acosh") {
        return std::acosh(a);
    }

    else if (function_name == "tan") {
        return std::tan(a);
    } else if (function_name == "atan") {
        return std::atan(a);
    } else if (function_name == "tanh") {
        return std::tanh(a);
    } else if (function_name == "atanh") {
        return std::atanh(a);
    }

    else if (function_name == "ctan") {
        return std::tan(half_pi - a);
    } else if (function_name == "actan") {
        return std::atan(half_pi - a);
    } else if (function_name == "ctanh") {
        return std::tanh(half_pi - a);
    } else if (function_name == "actanh") {
        return std::atanh(half_pi - a);
    }

    ASSERT(not stack.empty(),
           "Function `" << function_name << "` call is incorrect");

    const F b = stack.back();
    stack.pop_back();

    switch (function.type) {
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
        default:
            break;
    }

    if (function_name == "min") {
        return std::min(b, a);
    } else if (function_name == "max") {
        return std::max(b, a);
    } else if (function_name == "log") {
        return std::log(a) / std::log(b);
    } else if (function_name == "gcd") {
        return std::gcd(static_cast<int>(b), static_cast<int>(a));
    } else if (function_name == "lcm") {
        return std::lcm(static_cast<int>(b), static_cast<int>(a));
    } else if (function_name == "mod") {
        return std::fmod(b, a);
    } else if (function_name == "pow") {
        return std::pow(b, a);
    } else if (function_name == "add") {
        return b + a;
    } else if (function_name == "sub") {
        return b - a;
    } else if (function_name == "div") {
        return b / a;
    } else if (function_name == "mul") {
        return b * a;
    } else if (function_name == "add") {
        return b + a;
    }

    ASSERT(false, "Function `" << function_name << "` is not found");

    return std::nan("0");
}

template <class F, class It>
auto parse_num(It it) -> F {
    if (*it->lexeme_start == '0') {
        size_t val;
        const auto next = it->lexeme_start + 1;

        switch (*next) {
            case 'x':
                std::from_chars(it->lexeme_start + 2, it->lexeme_end, val, 16);
                return val;
            case 'o':
                std::from_chars(it->lexeme_start + 2, it->lexeme_end, val, 8);
                return val;
            case 'b':
                std::from_chars(it->lexeme_start + 2, it->lexeme_end, val, 2);
                return val;
        }
    }
    F val;
    std::from_chars(it->lexeme_start, it->lexeme_end, val);
    return val;
}

}  // namespace detail

template <class F, class It>
constexpr auto evaluate(It begin, It end) {
    using namespace detail;

    auto queue = parse(begin, end);

    std::vector<F> stack;

    for (auto it = queue.begin(); it != queue.end();) {
        if (it->type == Token<It>::Number) {
            stack.push_back(parse_num<F>(it));
            ++it;
            continue;
        }

        stack.push_back(perform<F, It>(*it, stack));
        ++it;
    }

    ASSERT(stack.size() == 1, "Reduntant values");

    return stack.back();
}

}  // namespace calc
