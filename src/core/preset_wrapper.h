#pragma once
#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace tehi {

struct DebugPreset {
    static void enter(const char* system, const char* op) {
        std::printf("[Debug] %s -> %s : entering\n", system, op);
    }
    static void ok(const char* system, const char* op) {
        std::printf("[Debug] %s -> %s : OK\n", system, op);
    }
    static void fail(const char* system, const char* op, const char* reason) {
        std::printf("[Debug] %s -> %s : FAILED - %s\n", system, op, reason);
    }
    static void warn(const char* system, const char* msg) {
        std::printf("[Debug] %s : WARNING - %s\n", system, msg);
    }
    static void info(const char* system, const char* msg) {
        std::printf("[Debug] %s : %s\n", system, msg);
    }
};

struct SelfHealPreset {
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
    static void healed(const char* system, const char* op) {
        std::printf("[SelfHeal] %s -> %s : healed\n", system, op);
    }
};

struct FilePreset {
    static bool exists(const char* path) {
        FILE* f = std::fopen(path, "r");
        if (f) { std::fclose(f); return true; }
        return false;
    }
    static bool safe_write(const char* path, const char* data, size_t len) {
        FILE* f = std::fopen(path, "w");
        if (!f) return false;
        size_t written = std::fwrite(data, 1, len, f);
        std::fclose(f);
        return written == len;
    }
    static bool safe_read(const char* path, char* buf, size_t maxlen) {
        FILE* f = std::fopen(path, "r");
        if (!f) return false;
        size_t read = std::fread(buf, 1, maxlen, f);
        buf[read < maxlen ? read : maxlen - 1] = '\0';
        std::fclose(f);
        return true;
    }
};

} // namespace tehi

// Universal debug macros
#define DEBUG_ENTER(system, op) tehi::DebugPreset::enter(system, op)
#define DEBUG_OK(system, op) tehi::DebugPreset::ok(system, op)
#define DEBUG_FAIL(system, op, reason) tehi::DebugPreset::fail(system, op, reason)
#define DEBUG_WARN(system, msg) tehi::DebugPreset::warn(system, msg)
#define DEBUG_INFO(system, msg) tehi::DebugPreset::info(system, msg)

// Universal self-heal macros
#define SELF_HEAL_ENTER(system, op) tehi::SelfHealPreset::enter(system, op)
#define SELF_HEAL_OK(system, op) tehi::SelfHealPreset::ok(system, op)
#define SELF_HEAL_FAIL(system, op, reason) tehi::SelfHealPreset::fail(system, op, reason)
#define SELF_HEAL_RECOVER(system, op) tehi::SelfHealPreset::recover(system, op)
#define SELF_HEAL_HEALED(system, op) tehi::SelfHealPreset::healed(system, op)

// Universal file safety macros
#define FILE_EXISTS(path) tehi::FilePreset::exists(path)
#define FILE_SAFE_WRITE(path, data, len) tehi::FilePreset::safe_write(path, data, len)
#define FILE_SAFE_READ(path, buf, maxlen) tehi::FilePreset::safe_read(path, buf, maxlen)

// Combined guard: debug + self-heal + auto-recover
#define PRESET_GUARD(system, op, expr, recover_expr) \
    do { \
        DEBUG_ENTER(system, op); \
        SELF_HEAL_ENTER(system, op); \
        if (!(expr)) { \
            DEBUG_FAIL(system, op, #expr); \
            SELF_HEAL_FAIL(system, op, #expr); \
            SELF_HEAL_RECOVER(system, op); \
            { recover_expr; } \
            SELF_HEAL_HEALED(system, op); \
        } else { \
            DEBUG_OK(system, op); \
            SELF_HEAL_OK(system, op); \
        } \
    } while(0)
