#pragma once

#include <iostream>

#ifndef NDEBUG
#define ASSERT(condition, message)                                             \
    do {                                                                       \
        if (!(condition)) {                                                    \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__   \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate();                                                  \
        }                                                                      \
    } while (false)
#else
#define ASSERT(condition, message)                                      \
    do {                                                                \
        if (!(condition)) {                                             \
            std::cerr << "[31mError occured: " << message << std::endl \
                      << "[0m";                                        \
            std::exit(1);                                               \
        }                                                               \
    } while (false)
#endif
