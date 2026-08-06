#include "companion.h"
#include <cstdio>

namespace tehi {

bool StoryCompanion::initialize() {
    std::snprintf(m_name, sizeof(m_name), "Lia");
    std::snprintf(m_voice, sizeof(m_voice), "Synthetic");
    std::printf("[Companion] %s initialized\n", m_name);
    return true;
}

void StoryCompanion::update(float player_health, float shield, float dt) {
    if (m_cooldown > 0.0f) { m_cooldown -= dt; return; }
    if (player_health < 0.25f) {
        say("Your vitals are critical. Find cover.");
        m_cooldown = 8.0f;
    } else if (shield < 0.1f && player_health < 0.5f) {
        say("Shields are down. I am rerouting power.");
        m_cooldown = 10.0f;
    }
}

void StoryCompanion::say(const char* line) {
    std::printf("[Companion] %s: %s\n", m_name, line);
}

} // namespace tehi
