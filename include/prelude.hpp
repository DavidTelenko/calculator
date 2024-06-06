#pragma once

#include <iostream>
#include <ranges>

#ifndef NDEBUG
#define ASSERT(condition, message)                                             \
    do {                                                                       \
        if (!(condition)) {                                                    \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__   \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            return std::nullopt;                                               \
        }                                                                      \
    } while (false)
#else
#define ASSERT(condition, message)                                      \
    do {                                                                \
        if (!(condition)) {                                             \
            std::cerr << "[31mError occured: " << message << std::endl \
                      << "[0m";                                        \
            return std::nullopt;                                        \
        }                                                               \
    } while (false)
#endif

namespace calc {

template <class T, class Out>
constexpr auto print(Out out, const T& val) -> Out {
    if constexpr (std::ranges::range<T>) {
        auto it = std::ranges::begin(val);
        out(*it);
        for (++it; it != std::ranges::end(val); ++it) {
            out(", ");
            out(*it);
        }
    } else {
        out(val);
    }
    return out;
}

constexpr double E = 2.718'281'828'459'045'235'40;
constexpr double E_GAMMA = 0.577'215'664'901'532'860'60;
constexpr double TAU = 6.283'185'307'179'586'231'99;
constexpr double PHI = 1.618'033'988'749'894'848'20;
constexpr double QUARTER_PI = 0.785'398'163'397'448'309'62;
constexpr double HALF_PI = 1.570'796'326'794'896'619'23;
constexpr double PI = 3.141'592'653'589'793'238'46;
constexpr double TWO_PI = 6.283'185'307'179'586'231'99;
constexpr double INV_PI = 0.318'309'886'183'790'671'54;
constexpr double INV_SQRT_PI = 1.128'379'167'095'512'573'90;
constexpr double INV_TWO_PI = 0.636'619'772'367'581'343'08;

}  // namespace calc
