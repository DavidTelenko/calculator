#pragma once

#include <iostream>
#include <iterator>
#include <string_view>
#include <unordered_map>

#include "prelude.hpp"

namespace calc {

template <class F, class It>
class Variables {
    using value_type = std::iter_value_t<It>;
    using str = std::basic_string<value_type>;
    using str_view = std::basic_string_view<value_type>;

   public:
    auto get(str_view name) -> std::optional<F> const {
        if (name == "e") {
            return calc::E;
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
        } else {
            const auto result = _registry.find(str(name));
            if (result != _registry.end()) {
                return result->second;
            }
        }
        return std::nullopt;
    }

    auto set(str_view name, F value) -> Variables& {
        _registry[str(name)] = value;
        return *this;
    }

    friend auto& operator<<(std::basic_ostream<std::iter_value_t<It>>& os,
                            const Variables& vars) {
        calc::print(
            [&os](auto&& e) { os << e.first << " = " << e.second << '\n'; },
            vars._registry);
        return os;
    }

   private:
    std::unordered_map<str, F> _registry;
};

}  // namespace calc
