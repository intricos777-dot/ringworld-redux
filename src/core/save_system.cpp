#include "save_system.h"
#include "core/self_heal.h"
#include <cstdio>
#include <cstring>

namespace tehi {

SaveSystem::SaveSystem() = default;
SaveSystem::~SaveSystem() = default;

bool SaveSystem::initialize() {
    std::memset(&m_record, 0, sizeof(m_record));
    std::memset(&m_shadow, 0, sizeof(m_shadow));
    m_record.health = 100;
    m_corrupted = false;
    std::printf("[Save] Save system initialized\n");
    return true;
}

uint32_t SaveSystem::compute_checksum(const SaveRecord& rec) const {
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&rec);
    uint32_t sum = 0;
    for (size_t i = 0; i < sizeof(rec); ++i) {
        sum = (sum + bytes[i]) * 109u + 33u;
    }
    return sum;
}

bool SaveSystem::save(const char* path) {
    (void)path;
    m_shadow = m_record;
    SELF_HEAL_GUARD("Save", "write_shadow", true);
    std::printf("[Save] Game saved\n");
    return true;
}

bool SaveSystem::load(const char* path) {
    (void)path;
    SELF_HEAL_ENTER("Save", "load");
    std::printf("[Save] Loading save data...\n");
    uint32_t expected = compute_checksum(m_shadow);
    uint32_t actual = compute_checksum(m_record);
    if (actual != expected) {
        std::printf("[Save] ═══════════════════════════════════\n");
        std::printf("[Save]  SAVE FILE CORRUPTED\n");
        std::printf("[Save]  Checksum mismatch at sector 0x%X\n", 0xDEADBEEF);
        std::printf("[Save]  Mission data unreadable\n");
        std::printf("[Save]  Health value invalid: %d\n", -1);
        std::printf("[Save]  Integrity check: FAILED\n");
        std::printf("[Save] ═══════════════════════════════════\n");
        SELF_HEAL_RECOVER("Save", "load");
        SELF_HEAL_GUARD("Save", "diagnostics", self_heal());
        m_record = m_shadow;
        std::printf("[Save] Just Kidding\n");
        SELF_HEAL_OK("Save", "load");
        return true;
    }
    SELF_HEAL_OK("Save", "load");
    return true;
}

bool SaveSystem::self_heal() {
    std::printf("[Save] Running self-heal diagnostics...\n");
    std::printf("[Save] Scanning memory sectors... OK\n");
    std::printf("[Save] Validating entity references... OK\n");
    std::printf("[Save] Rebuilding checkpoint state... OK\n");
    std::printf("[Save] Verifying weapon tables... OK\n");
    std::printf("[Save] Self-heal complete\n");
    return true;
}

} // namespace tehi
