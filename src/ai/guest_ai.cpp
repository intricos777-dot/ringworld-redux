#include "guest_ai.h"
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cstdlib>

namespace tehi {

static const GuestAITraits TYPE_TRAITS[] = {
    { 140.0f, 120.0f, 18.0f, 0.9f, 6.0f, "Bulwark" },
    { 90.0f, 30.0f, 35.0f, 1.4f, 3.0f, "Skirmisher" },
    { 70.0f, 20.0f, 55.0f, 1.8f, 2.0f, "Assassin" },
};

static const GuestAIPersonality PERSONALITIES[] = {
    { "Sgt. Major Rook", "Southern US", "Short, brutal, commands respect", 0.9f, 0.3f, 0.2f },
    { "Doc Meridian", "Irish-American", "Calm, reassuring, dry wit", 0.4f, 0.7f, 0.6f },
    { "Chief Surge", "Brooklyn", "Loud, fast-talking, jokes under fire", 0.8f, 0.4f, 0.9f },
    { "Wraith", "Midwestern", "Quiet, cryptic, speaks in metaphors", 0.7f, 0.8f, 0.3f },
    { "Harbor", "New England", "Precise, strategic, dry sarcasm", 0.5f, 0.6f, 0.5f },
    { "Anvil", "Texas", "Boisterous, protective, treats squad like family", 0.85f, 0.35f, 0.7f },
};

GuestAI::GuestAI() = default;
GuestAI::~GuestAI() = default;

bool GuestAI::initialize(uint32_t id, GuestAIType type, const char* personality_name) {
    m_id = id;
    m_type = type;
    const auto& traits = TYPE_TRAITS[(uint32_t)type];
    m_health = traits.health;
    m_shield = traits.shield;
    m_damage = traits.damage;
    m_move_speed = traits.move_speed;
    m_support_radius = traits.support_radius;
    m_deployed = false;
    m_cooldown = 0.0f;

    const GuestAIPersonality* pers = nullptr;
    for (const auto& p : PERSONALITIES) {
        if (std::strcmp(p.name, personality_name) == 0) {
            pers = &p;
            break;
        }
    }
    if (!pers) {
        pers = &PERSONALITIES[0];
    }

    std::snprintf(m_name, sizeof(m_name), "%s", pers->name);
    std::snprintf(m_accent, sizeof(m_accent), "%s", pers->accent_region);
    std::snprintf(m_speaking_style, sizeof(m_speaking_style), "%s", pers->speaking_style);

    std::printf("[GuestAI] Initialized %s (%s %s) health=%.1f shield=%.1f damage=%.1f\n",
        m_name, m_accent, traits.role, m_health, m_shield, m_damage);
    return true;
}

void GuestAI::update(float dt) {
    if (!m_deployed) return;
    if (m_cooldown > 0.0f) { m_cooldown -= dt; return; }
    // Stub: movement, aggro, support actions
    (void)dt;
}

void GuestAI::deploy() {
    m_deployed = true;
    std::printf("[GuestAI] %s deployed\n", m_name);
    say("I'm on the line. Keep low and watch your sectors.");
}

void GuestAI::retreat() {
    m_deployed = false;
    std::printf("[GuestAI] %s retreating\n", m_name);
    say("Moving to rear. Rejoining shortly.");
}

void GuestAI::say(const char* line) const {
    std::printf("[GuestAI] %s [%s]: %s\n", m_name, m_accent, line);
}

} // namespace tehi
