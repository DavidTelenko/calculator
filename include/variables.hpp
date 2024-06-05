#pragma once

#include "prelude.hpp"

namespace calc {

template <class F, class It>
constexpr auto built_in_variables(std::basic_string_view<It> name)
    -> std::optional<F> {
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
        return std::nullopt;
    }
}

}  // namespace calc
