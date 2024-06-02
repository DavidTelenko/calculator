#include <iomanip>
#include <iostream>
#include <lexer.hpp>
#include <ostream>
#include <string_view>

#include "evaluator.hpp"

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

int main(int argc, char* argv[]) {
    const std::string_view expr = argv[1];

    std::cout << getStyled(calc::evaluate<double>(expr.begin(), expr.end()))
              << std::flush;

    return 0;
}
