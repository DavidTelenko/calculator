#include <iomanip>
#include <iostream>
#include <lexer.hpp>
#include <ostream>
#include <string_view>

#include "evaluator.hpp"
#include "prelude.hpp"

using F = double;

auto getStyled(double num) {
    struct ThousandsSep : std::numpunct<char> {
        char do_thousands_sep() const { return '\''; }
        std::string do_grouping() const { return "\3"; }
    };

    std::stringstream ss;
    ss.imbue(std::locale(ss.getloc(), new ThousandsSep));

    ss << (num < 1e40 ? std::fixed : std::scientific) << std::setprecision(10)
       << num;

    auto numStr = ss.str();

    auto dot = numStr.find('.');
    if (dot != std::string::npos) {
        while (numStr.back() == '0') numStr.pop_back();
        if (numStr.back() == '.') numStr.pop_back();
    }

    return numStr;
}

template <class It>
constexpr auto built_in_variables(std::basic_string_view<It> name) -> F {
    if (name == "e") {
        return calc::E;
    } else if (name == "log2e") {
        return calc::LOG2E;
    } else if (name == "log10e") {
        return calc::LOG10E;
    } else if (name == "ln2") {
        return calc::LN2;
    } else if (name == "ln10") {
        return calc::LN10;
    } else if (name == "e_gamma") {
        return calc::E_GAMMA;
    } else if (name == "tau") {
        return calc::TAU;
    } else if (name == "phi") {
        return calc::PHI;
    } else if (name == "quarter_pi") {
        return calc::QUARTER_PI;
    } else if (name == "half_pi") {
        return calc::HALF_PI;
    } else if (name == "pi") {
        return calc::PI;
    } else if (name == "two_pi") {
        return calc::TWO_PI;
    } else if (name == "inv_pi") {
        return calc::INV_PI;
    } else if (name == "inv_sqrt_pi") {
        return calc::INV_SQRT_PI;
    } else if (name == "inv_two_pi") {
        return calc::INV_TWO_PI;
    } else if (name == "sqrt2") {
        return calc::SQRT2;
    } else if (name == "sqrt3") {
        return calc::SQRT3;
    } else if (name == "inv_sqrt2") {
        return calc::INV_SQRT2;
    } else if (name == "inv_sqrt3") {
        return calc::INV_SQRT3;
    } else {
        return 0;
    }
}

int main(int argc, char* argv[]) {
    const std::basic_string_view expr = argv[1];

    const auto evaluated =
        calc::evaluate<F>(expr.begin(), expr.end(), built_in_variables<char>);

    std::cout << getStyled(evaluated) << std::flush;

    return 0;
}
