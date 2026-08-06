#include "achievements.h"
#include <cstdio>

namespace tehi {

bool AchievementSystem::initialize() {
    m_records = {
        { Achievement::ACHIEVEMENT_CLEAR_AWAKENING, "First Light", "Complete Awakening" },
        { Achievement::ACHIEVEMENT_CLEAR_FIRST_CONTACT, "First Blood", "Complete First Contact" },
        { Achievement::ACHIEVEMENT_CLEAR_TOWER_OF_BABEL, "High Ground", "Complete Tower of Babel" },
        { Achievement::ACHIEVEMENT_CLEAR_FOUNDATION, "Bottom of the Ring", "Complete Foundation" },
        { Achievement::ACHIEVEMENT_CLEAR_ALL_MISSIONS, "Veteran", "Complete all missions" },
        { Achievement::ACHIEVEMENT_LEGEND_COMPLETE, "Legend", "Complete any mission on Legend" },
    };
    return true;
}

AchievementSystem& AchievementSystem::instance() {
    static AchievementSystem instance;
    return instance;
}

void AchievementSystem::unlock(Achievement id) {
    for (auto& r : m_records) {
        if (r.id == id && !r.unlocked) {
            r.unlocked = true;
            std::printf("[Achievement] Unlocked: %s — %s\n", r.name, r.description);
        }
    }
}

bool AchievementSystem::is_unlocked(Achievement id) const {
    for (const auto& r : m_records) {
        if (r.id == id) return r.unlocked;
    }
    return false;
}

void AchievementSystem::update_enemy_count(uint32_t active_enemies) {
    m_active_enemies = active_enemies;
}

} // namespace tehi
