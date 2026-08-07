#include "level_modifier.h"
#include <cstdio>
#include <cstring>

namespace tehi {

static const EnemyUpgrade UPGRADE_TABLE[] = {
    { "Mutated Grunt", (uint32_t)EnemyClass::Grunt, EnemyClass::MutatedGrunt, 40.0f, 10.0f, 8.0f,
      "regenerate", "Genetically enhanced grunt with flesh-regeneration weave." },
    { "Mutated Elite", (uint32_t)EnemyClass::Elite, EnemyClass::MutatedGrunt, 50.0f, 20.0f, 12.0f,
      "berserk", "Elite bonded with invasive growth-clusters and adrenal overclocks." },
    { "Integrated Seeker", (uint32_t)EnemyClass::SeekerBot, EnemyClass::TechnoIntegratedElite, 30.0f, 40.0f, 15.0f,
      "shock", "Seeker bot fused with alien neural tissue and capacitor plating." },
    { "Phantom Stalker", (uint32_t)EnemyClass::Elite, EnemyClass::PhantomStalker, 35.0f, 50.0f, 20.0f,
      "cloak", "Elite equipped with experimental phase-cloak and disruptor blade." },
    { "Enhanced Sentry", (uint32_t)EnemyClass::SeekerBot, EnemyClass::EnhancedSentry, 80.0f, 20.0f, 10.0f,
      "turret", "Sentry platform with reinforced chassis and auto-targeting." },
};

static const char* ENEMY_NAMES[] = {
    "Grunt", "Elite", "Seeker Bot", "Commander",
    "Mutated Grunt", "Techno-Integrated Elite",
    "Enhanced Sentry", "Phantom Stalker"
};

LevelModifierSystem::LevelModifierSystem() = default;
LevelModifierSystem::~LevelModifierSystem() = default;

bool LevelModifierSystem::initialize() {
    std::printf("[LevelMod] System initialized\n");
    return true;
}

LevelModifierSystem& get_level_modifier_system() {
    static LevelModifierSystem instance;
    return instance;
}

EnemyClass LevelModifierSystem::get_upgraded_enemy(uint32_t level, uint32_t base_type) const {
    if (level <= 7) return (EnemyClass)base_type;
    EnemyClass base = (EnemyClass)base_type;
    return pick_upgrade(level, base);
}

const char* LevelModifierSystem::get_enemy_name(EnemyClass type) const {
    uint32_t idx = (uint32_t)type;
    if (idx >= sizeof(ENEMY_NAMES) / sizeof(ENEMY_NAMES[0])) return "Unknown";
    return ENEMY_NAMES[idx];
}

float LevelModifierSystem::get_damage_modifier(EnemyClass type) const {
    switch (type) {
        case EnemyClass::MutatedGrunt: return 1.2f;
        case EnemyClass::TechnoIntegratedElite: return 1.4f;
        case EnemyClass::PhantomStalker: return 1.6f;
        case EnemyClass::EnhancedSentry: return 1.1f;
        default: return 1.0f;
    }
}

float LevelModifierSystem::get_shield_modifier(EnemyClass type) const {
    switch (type) {
        case EnemyClass::MutatedGrunt: return 1.3f;
        case EnemyClass::TechnoIntegratedElite: return 1.5f;
        case EnemyClass::PhantomStalker: return 1.7f;
        case EnemyClass::EnhancedSentry: return 1.2f;
        default: return 1.0f;
    }
}

EnemyClass LevelModifierSystem::pick_upgrade(uint32_t level, EnemyClass base) const {
    if (level >= 12) {
        switch (base) {
            case EnemyClass::Elite: return EnemyClass::PhantomStalker;
            case EnemyClass::SeekerBot: return EnemyClass::EnhancedSentry;
            default: break;
        }
    }
    if (level >= 10) {
        switch (base) {
            case EnemyClass::Elite: return EnemyClass::TechnoIntegratedElite;
            case EnemyClass::SeekerBot: return EnemyClass::TechnoIntegratedElite;
            default: break;
        }
    }
    if (level >= 8) {
        switch (base) {
            case EnemyClass::Grunt: return EnemyClass::MutatedGrunt;
            case EnemyClass::Elite: return EnemyClass::MutatedGrunt;
            default: break;
        }
    }
    return base;
}

} // namespace tehi
