#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace tehi {

enum class Achievement : uint32_t {
    ACHIEVEMENT_CLEAR_AWAKENING = 0,
    ACHIEVEMENT_CLEAR_FIRST_CONTACT,
    ACHIEVEMENT_CLEAR_TOWER_OF_BABEL,
    ACHIEVEMENT_CLEAR_FOUNDATION,
    ACHIEVEMENT_CLEAR_ALL_MISSIONS,
    ACHIEVEMENT_LEGEND_COMPLETE,
    ACHIEVEMENT_COUNT
};

struct AchievementRecord {
    Achievement id;
    const char* name;
    const char* description;
    bool unlocked = false;
};

class AchievementSystem {
public:
    AchievementSystem() = default;
    ~AchievementSystem() = default;
    bool initialize();
    void unlock(Achievement id);
    bool is_unlocked(Achievement id) const;
    const std::vector<AchievementRecord>& records() const { return m_records; }
    void update_enemy_count(uint32_t active_enemies);
private:
    std::vector<AchievementRecord> m_records;
    uint32_t m_active_enemies = 0;
};

} // namespace tehi
