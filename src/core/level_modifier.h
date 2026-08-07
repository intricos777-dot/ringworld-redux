#pragma once
#include <cstdint>
#include <string>

namespace tehi {

enum class EnemyClass : uint32_t {
    Grunt = 0,
    Elite,
    SeekerBot,
    Commander,
    MutatedGrunt,
    TechnoIntegratedElite,
    EnhancedSentry,
    PhantomStalker
};

struct EnemyUpgrade {
    const char* name;
    uint32_t base_type;
    EnemyClass upgraded_type;
    float health_bonus;
    float shield_bonus;
    float damage_bonus;
    const char* ability;
    const char* description;
};

class LevelModifierSystem {
public:
    LevelModifierSystem();
    ~LevelModifierSystem();
    bool initialize();
    EnemyClass get_upgraded_enemy(uint32_t level, uint32_t base_type) const;
    const char* get_enemy_name(EnemyClass type) const;
    float get_damage_modifier(EnemyClass type) const;
    float get_shield_modifier(EnemyClass type) const;
private:
    EnemyClass pick_upgrade(uint32_t level, EnemyClass base) const;
};

LevelModifierSystem& get_level_modifier_system();

} // namespace tehi
