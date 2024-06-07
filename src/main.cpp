#include <iomanip>
#include <iostream>
#include <ostream>
#include <string_view>

#include "evaluator.hpp"
#include "prelude.hpp"
#include "variables.hpp"

template <class F>
auto get_styled(F num) {
    struct ThousandsSep : std::numpunct<char> {
        char do_thousands_sep() const { return '\''; }
        std::string do_grouping() const { return "\3"; }
    };

    std::stringstream ss;
    ss.imbue(std::locale(ss.getloc(), new ThousandsSep));

    ss << (num < 1e40 or num < 1e-40 ? std::fixed : std::scientific)
       << std::setprecision(15) << num;

    auto numStr = ss.str();

    auto dot = numStr.find('.');
    if (dot != std::string::npos) {
        while (numStr.back() == '0') numStr.pop_back();
        if (numStr.back() == '.') numStr.pop_back();
    }

    return numStr;
}

auto repl() -> int {
    using F = double;

    std::string buffer;
    calc::Variables<F, decltype(buffer)::iterator> variables;

    for (;;) {
        std::cout << "> ";
        std::getline(std::cin, buffer);

        if (buffer.empty()) {
            continue;
        }

        if (buffer == "exit" or buffer == "q" or buffer == "quit") {
            std::cout << variables;
            return 0;
        }

        const auto result = calc::evaluate<F>(
            buffer.data(), buffer.data() + buffer.size(), variables);

        if (not result) {
            continue;
        }

        std::cout << "= [33m" << get_styled(*result) << "[0m\n\n";
    }

    return 1;
}

int main(int argc, char* argv[]) {
    using F = double;

    calc::Variables<F, char*> variables;

    if (argc == 1) {
        return repl();
    }

    const std::basic_string_view expr = argv[1];

    const auto result = calc::evaluate<F>(expr.begin(), expr.end(), variables);

    if (not result) {
        return 1;
    }

    std::cout << get_styled(*result) << std::flush;

    return 0;
}
