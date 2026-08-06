#pragma once
#include <cstdio>
#include <cstring>

namespace tehi {

struct SelfHealLog {
    static void enter(const char* system, const char* op) {
        std::printf("[SelfHeal] %s -> %s : entering\n", system, op);
    }
    static void ok(const char* system, const char* op) {
        std::printf("[SelfHeal] %s -> %s : OK\n", system, op);
    }
    static void fail(const char* system, const char* op, const char* reason) {
        std::printf("[SelfHeal] %s -> %s : FAILED - %s\n", system, op, reason);
    }
    static void recover(const char* system, const char* op) {
        std::printf("[SelfHeal] %s -> %s : recovering...\n", system, op);
    }
};

#define SELF_HEAL_ENTER(system, op) tehi::SelfHealLog::enter(system, op)
#define SELF_HEAL_OK(system, op) tehi::SelfHealLog::ok(system, op)
#define SELF_HEAL_FAIL(system, op, reason) tehi::SelfHealLog::fail(system, op, reason)
#define SELF_HEAL_RECOVER(system, op) tehi::SelfHealLog::recover(system, op)

#define SELF_HEAL_GUARD(system, op, expr) \
    do { \
        SELF_HEAL_ENTER(system, op); \
        if (!(expr)) { \
            SELF_HEAL_FAIL(system, op, #expr); \
            SELF_HEAL_RECOVER(system, op); \
        } else { \
            SELF_HEAL_OK(system, op); \
        } \
    } while(0)

} // namespace tehi
