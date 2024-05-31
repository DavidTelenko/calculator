#include <iomanip>
#include <iostream>
#include <iterator>
#include <lexer.hpp>
#include <ostream>
#include <string_view>
#include <vector>

int main(int argc, char* argv[]) {
    const std::string_view expr =
        "2 + 22.2 - 1.1 * (hello + f(10)) / (9 + 10 / 2)";

    std::cout << std::quoted(expr) << std::endl;

    std::vector<calc::Token<typename decltype(expr)::iterator>> tokens;

    calc::lex_all(expr.begin(), expr.end(), std::back_inserter(tokens));

    for (auto token : tokens) {
        std::cout << token.get_name() << " "
                  << std::quoted(
                         std::string_view(token.lexeme_start, token.lexeme_end))
                  << "\n";
    }

    std::cout << std::flush;

    return 0;
}
