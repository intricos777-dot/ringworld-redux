#include "easter_eggs.h"
#include <cstdio>
#include <ctime>

namespace tehi {

EasterEggSystem::EasterEggSystem() : m_rng((unsigned)std::time(nullptr)), m_dist(0.0f, 1.0f) {
    m_eggs = {
        { EasterEggType::WarthogLaunch, "Warthog Launch", "Vehicle launches into the sky", 0.02f },
        { EasterEggType::BansheeSuicide, "Banshee Suicide", "Banshee explodes on light impact", 0.03f },
        { EasterEggType::InfiniteAmmo, "Infinite Ammo", "Weapon never reloads", 0.01f },
        { EasterEggType::GhostFlip, "Ghost Flip", "Ghost performs backflip", 0.04f },
        { EasterEggType::InvisibleScarab, "Invisible Scarab", "Scarab becomes invisible", 0.01f },
        { EasterEggType::RocketJump, "Rocket Jump", "Rocket propels player upward", 0.05f },
        { EasterEggType::SpartanFlip, "Spartan Flip", "Player character flips end-over-end", 0.03f },
        { EasterEggType::BansheeBoost, "Banshee Boost", "Banshee gets extreme speed boost", 0.04f },
        { EasterEggType::MeleeLunge, "Melee Lunge", "Melee sends player flying forward", 0.03f },
        { EasterEggType::GlitchText, "Glitch Text", "Screen displays corrupted text", 0.02f },
    };
}

void EasterEggSystem::update(float dt) {
    if (!m_enabled) return;
    if (m_cooldown > 0.0f) m_cooldown -= dt;
}

bool EasterEggSystem::try_trigger() {
    if (!m_enabled || m_cooldown > 0.0f) return false;
    float roll = m_dist(m_rng);
    for (const auto& egg : m_eggs) {
        if (roll < egg.chance) {
            trigger(egg.type);
            m_cooldown = 10.0f;
            return true;
        }
    }
    return false;
}

void EasterEggSystem::trigger(EasterEggType type) {
    const char* name = "Unknown";
    for (const auto& egg : m_eggs) {
        if (egg.type == type) { name = egg.name; break; }
    }
    std::printf("[EasterEgg] Triggered: %s\n", name);
    switch (type) {
        case EasterEggType::WarthogLaunch:
            std::printf("[EasterEgg] Warthog Launch: Vehicle launched into sky!\n");
            break;
        case EasterEggType::BansheeSuicide:
            std::printf("[EasterEgg] Banshee Suicide: Banshee exploded on impact!\n");
            break;
        case EasterEggType::InfiniteAmmo:
            std::printf("[EasterEgg] Infinite Ammo: Magazine never depletes!\n");
            break;
        case EasterEggType::GhostFlip:
            std::printf("[EasterEgg] Ghost Flip: Ghost performed backflip!\n");
            break;
        case EasterEggType::InvisibleScarab:
            std::printf("[EasterEgg] Invisible Scarab: Scarab vanished!\n");
            break;
        case EasterEggType::RocketJump:
            std::printf("[EasterEgg] Rocket Jump: Rocket blast sent you flying!\n");
            break;
        case EasterEggType::SpartanFlip:
            std::printf("[EasterEgg] Spartan Flip: Spartan flipped end-over-end!\n");
            break;
        case EasterEggType::BansheeBoost:
            std::printf("[EasterEgg] Banshee Boost: Extreme speed activated!\n");
            break;
        case EasterEggType::MeleeLunge:
            std::printf("[EasterEgg] Melee Lunge: Melee sent you flying!\n");
            break;
        case EasterEggType::GlitchText:
            std::printf("[EasterEgg] Glitch Text: █▒░█▓▒░█▓▒░█▓▒░\n");
            break;
    }
}

EasterEggSystem& get_easter_egg_system() {
    static EasterEggSystem instance;
    return instance;
}

} // namespace tehi
