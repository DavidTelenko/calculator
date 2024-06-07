#pragma once

#include <charconv>
#include <cmath>
#include <iterator>
#include <numeric>
#include <optional>
#include <string_view>
#include <utility>

#include "lexer.hpp"
#include "parser.hpp"
#include "prelude.hpp"

namespace calc {

template <class T, class F, class It>
concept variables_handle = requires(
    T variables, std::basic_string_view<std::iter_value_t<It>> str, F num) {
    std::floating_point<decltype(variables.get(str))>;
    variables.set(str, num);
};

namespace detail {

template <class It>
using str_view = typename std::basic_string_view<std::iter_value_t<It>>;

template <class It>
using variables_stack = std::vector<str_view<It>>;

constexpr auto delimited() {
    return [](auto&& v) { std::cout << '`' << v << "` "; };
}

template <class T>
constexpr auto make_delimited_print(const T& val) {
    return calc::print_proxy(delimited(), val);
}

template <class F, class It>
auto parse_num(const Token<It>& token) -> F {
    if (*token.lexeme_start == '0') {
        size_t val;
        const auto next = token.lexeme_start + 1;

        switch (*next) {
            case 'x':
                std::from_chars(token.lexeme_start + 2, token.lexeme_end, val,
                                16);
                return static_cast<F>(val);
            case 'o':
                std::from_chars(token.lexeme_start + 2, token.lexeme_end, val,
                                8);
                return static_cast<F>(val);
            case 'b':
                std::from_chars(token.lexeme_start + 2, token.lexeme_end, val,
                                2);
                return static_cast<F>(val);
        }
    }
    F val;
    std::from_chars(token.lexeme_start, token.lexeme_end, val);
    return val;
}

template <class F, class Fn, std::size_t... I>
auto _eval_fn(Fn fn, auto name, std::vector<F>& stack,
              std::index_sequence<I...>) -> std::optional<F> {
    ASSERT(stack.size() >= sizeof...(I),
           "Invalid call to `" << name << "` function");
    const auto result = fn(stack[stack.size() - sizeof...(I) + I]...);
    for (size_t i = 0; i < sizeof...(I); ++i) {
        stack.pop_back();
    }
    stack.push_back(result);
    return result;
}

template <size_t params, class F>
auto eval_fn(auto fn, auto name, std::vector<F>& stack) -> std::optional<F> {
    return _eval_fn<F>(fn, name, stack, std::make_index_sequence<params>{});
}

template <class It, class F, class Fn>
auto try_eval_unary_fn(typename Token<It>::Type, str_view<It> name,
                       std::vector<F>& stack, Fn not_found)
    -> std::optional<F> {
    if (name == "sqrt") {
        return eval_fn<1>([](F a) { return std::sqrt(a); }, name, stack);
    } else if (name == "cbrt") {
        return eval_fn<1>([](F a) { return std::cbrt(a); }, name, stack);
    }

    else if (name == "abs") {
        return eval_fn<1>([](F a) { return std::fabs(a); }, name, stack);
    }

    else if (name == "ln") {
        return eval_fn<1>([](F a) { return std::log(a); }, name, stack);
    } else if (name == "lg") {
        return eval_fn<1>([](F a) { return std::log10(a); }, name, stack);
    } else if (name == "exp") {
        return eval_fn<1>([](F a) { return std::exp(a); }, name, stack);
    }

    else if (name == "ceil") {
        return eval_fn<1>([](F a) { return std::ceil(a); }, name, stack);
    } else if (name == "floor") {
        return eval_fn<1>([](F a) { return std::floor(a); }, name, stack);
    } else if (name == "round") {
        return eval_fn<1>([](F a) { return std::round(a); }, name, stack);
    } else if (name == "trunc") {
        return eval_fn<1>([](F a) { return std::trunc(a); }, name, stack);
    }

    else if (name == "sin") {
        return eval_fn<1>([](F a) { return std::sin(a); }, name, stack);
    } else if (name == "asin") {
        return eval_fn<1>([](F a) { return std::asin(a); }, name, stack);
    } else if (name == "sinh") {
        return eval_fn<1>([](F a) { return std::sinh(a); }, name, stack);
    } else if (name == "asinh") {
        return eval_fn<1>([](F a) { return std::asinh(a); }, name, stack);
    }

    else if (name == "cos") {
        return eval_fn<1>([](F a) { return std::cos(a); }, name, stack);
    } else if (name == "acos") {
        return eval_fn<1>([](F a) { return std::acos(a); }, name, stack);
    } else if (name == "cosh") {
        return eval_fn<1>([](F a) { return std::cosh(a); }, name, stack);
    } else if (name == "acosh") {
        return eval_fn<1>([](F a) { return std::acosh(a); }, name, stack);
    }

    else if (name == "tan") {
        return eval_fn<1>([](F a) { return std::tan(a); }, name, stack);
    } else if (name == "atan") {
        return eval_fn<1>([](F a) { return std::atan(a); }, name, stack);
    } else if (name == "tanh") {
        return eval_fn<1>([](F a) { return std::tanh(a); }, name, stack);
    } else if (name == "atanh") {
        return eval_fn<1>([](F a) { return std::atanh(a); }, name, stack);
    }

    return not_found();
}

template <class It, class F, class Fn>
auto try_eval_binary_fn(typename Token<It>::Type type, str_view<It> name,
                        std::vector<F>& stack, Fn not_found)
    -> std::optional<F> {
    switch (type) {
        case Token<It>::Add:
            return eval_fn<2>([](auto a, auto b) { return a + b; }, name,
                              stack);
        case Token<It>::Sub:
            return eval_fn<2>([](auto a, auto b) { return a - b; }, name,
                              stack);
        case Token<It>::Mul:
            return eval_fn<2>([](auto a, auto b) { return a * b; }, name,
                              stack);
        case Token<It>::Div:
            return eval_fn<2>([](auto a, auto b) { return a / b; }, name,
                              stack);
        case Token<It>::Mod:
            return eval_fn<2>(std::fmod<F, F>, name, stack);
        case Token<It>::Pow:
            return eval_fn<2>(std::pow<F, F>, name, stack);
        case Token<It>::LessThan:
            return eval_fn<2>([](auto a, auto b) { return a < b; }, name,
                              stack);
        case Token<It>::LessEquals:
            return eval_fn<2>([](auto a, auto b) { return a <= b; }, name,
                              stack);
        case Token<It>::GreaterThan:
            return eval_fn<2>([](auto a, auto b) { return a > b; }, name,
                              stack);
        case Token<It>::GreaterEquals:
            return eval_fn<2>([](auto a, auto b) { return a >= b; }, name,
                              stack);
        case Token<It>::Equals:
            return eval_fn<2>(
                [](auto a, auto b) { return std::abs(a - b) <= 1e-40; }, name,
                stack);
        default:
            break;
    }

    if (name == "min") {
        return eval_fn<2>(std::min<F>, name, stack);
    } else if (name == "max") {
        return eval_fn<2>(std::max<F>, name, stack);
    } else if (name == "log") {
        return eval_fn<2>(
            [](auto a, auto b) { return std::log(a) / std::log(b); }, name,
            stack);
    } else if (name == "gcd") {
        return eval_fn<2>(
            [](auto a, auto b) {
                return std::gcd(static_cast<int>(a), static_cast<int>(b));
            },
            name, stack);
    } else if (name == "lcm") {
        return eval_fn<2>(
            [](auto a, auto b) {
                return std::lcm(static_cast<int>(a), static_cast<int>(b));
            },
            name, stack);
    } else if (name == "or") {
        return eval_fn<2>(
            [](auto a, auto b) {
                return static_cast<int>(a) or static_cast<int>(b);
            },
            name, stack);
    } else if (name == "and") {
        return eval_fn<2>(
            [](auto a, auto b) {
                return static_cast<int>(a) and static_cast<int>(b);
            },
            name, stack);
    } else if (name == "xor") {
        return eval_fn<2>(
            [](auto a, auto b) {
                return static_cast<int>(a) xor static_cast<int>(b);
            },
            name, stack);
    } else if (name == "add") {
        return eval_fn<2>([](auto a, auto b) { return a + b; }, name, stack);
    } else if (name == "sub") {
        return eval_fn<2>([](auto a, auto b) { return a - b; }, name, stack);
    } else if (name == "div") {
        return eval_fn<2>([](auto a, auto b) { return a / b; }, name, stack);
    } else if (name == "mul") {
        return eval_fn<2>([](auto a, auto b) { return a * b; }, name, stack);
    } else if (name == "mod") {
        return eval_fn<2>(std::fmod<F, F>, name, stack);
    } else if (name == "pow") {
        return eval_fn<2>(std::pow<F, F>, name, stack);
    }

    return not_found();
}

template <class It, class F, class Fn>
auto try_eval_fn(typename Token<It>::Type type, str_view<It> name,
                 std::vector<F>& stack, Fn not_found) -> std::optional<F> {
    return try_eval_unary_fn<It>(type, name, stack, [&]() {
        return try_eval_binary_fn<It>(type, name, stack, not_found);
    });
}

template <class F, class It>
constexpr auto perform(const Token<It>& identifier, std::vector<F>& stack,
                       variables_stack<It>& variables_stack,
                       variables_handle<F, It> auto& vars)
    -> std::optional<bool> {
    if (identifier.type == Token<It>::Number) {
        stack.push_back(parse_num<F>(identifier));
        return true;
    }

    const str_view<It> identifier_name{identifier.lexeme_start,
                                       identifier.lexeme_end};

    const auto variable = vars.get(identifier_name);

    if (variable) {
        stack.push_back(*variable);
        return true;
    }

    if (identifier.type == Token<It>::Assign) {
        ASSERT(not variables_stack.empty(), "Invalid assignment target");
        ASSERT(not stack.empty(), "Empty assignment value");

        vars.set(variables_stack.back(), stack.back());
        variables_stack.pop_back();

        return true;
    }

    if (not try_eval_fn<It>(identifier.type, identifier_name, stack,
                            [&variables_stack, &identifier_name]() {
                                variables_stack.push_back(identifier_name);
                                return std::make_optional(0);
                            })) {
        return std::nullopt;
    }

    return true;
}

}  // namespace detail

template <class F, class It>
constexpr auto evaluate(It begin, It end,
                        variables_handle<F, It> auto& variables)
    -> std::optional<F> {
    using namespace detail;

    const auto queue = parse(begin, end);

    if (not queue) {
        return std::nullopt;
    }

    // calc::print([](auto&& v) { std::cout << v << " "; }, *queue);

    std::vector<F> stack;
    detail::variables_stack<It> runtime_variables;

    for (auto&& token : *queue) {
        if (not perform<F, It>(token, stack, runtime_variables, variables)) {
            break;
        }
    }

    ASSERT(runtime_variables.empty(),
           "Undefinded variables " << make_delimited_print(runtime_variables));
    ASSERT(not stack.empty(), "Empty statement");
    ASSERT(stack.size() == 1, "Reduntant values");

    return stack.back();
}

}  // namespace calc
