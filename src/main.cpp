#include <iomanip>
#include <iostream>
#include <lexer.hpp>
#include <ostream>
#include <string_view>

int main(int argc, char* argv[]) {
    const std::string_view expr =
        "2 + 22.2 - 1.1 * (hello + f(10)) / (9 + 10 / 2)";

    std::cout << std::quoted(expr) << std::endl;

    for (auto token : calc::lex_all(expr.begin(), expr.end())) {
        std::cout << token.get_name() << " "
                  << std::quoted(
                         std::string_view(token.lexeme_start, token.lexeme_end))
                  << "\n";
    }

    std::cout << std::flush;

    return 0;
}
