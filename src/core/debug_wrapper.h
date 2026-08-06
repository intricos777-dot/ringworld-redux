#pragma once
#include <cstdio>

#define DEBUG_WRAP(system, call) \
    std::printf("[Debug] %s -> " call " : entering\n", system); \
    call; \
    std::printf("[Debug] %s -> " call " : exited OK\n", system)

#define DEBUG_LOG(fmt, ...) std::printf("[Debug] " fmt "\n", __VA_ARGS__)

#define SAFE_CALL(expr) \
    do { \
        if (!(expr)) { \
            std::printf("[Debug] FAILED: %s in %s:%d\n", #expr, __FILE__, __LINE__); \
        } else { \
            std::printf("[Debug] OK: %s in %s:%d\n", #expr, __FILE__, __LINE__); \
        } \
    } while(0)
