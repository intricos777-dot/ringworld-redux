#pragma once
#include <cstdint>
#include <string>

namespace tehi {

enum class BossAbility : uint32_t {
    None = 0,
    ShieldDrainPulse,
    SummonMinions,
    HeavyArmor,
    EnergyBeam,
    EMPBurst,
    CloakPhase,
    SentryDeploy
};

enum class BossPhase : uint32_t {
    Phase1 = 0,
    Phase2,
    Phase3,
    Defeated
};

struct BossTechProfile {
    const char* name;
    BossAbility primary;
    BossAbility secondary;
    BossAbility ultimate;
    float armor_rating;
    float buff_radius;
    const char* description;
};

class BossAlien {
public:
    BossAlien();
    ~BossAlien();
    bool initialize(uint32_t entity_id, uint32_t boss_type);
    void update(float dt, class World* world);
    void take_damage(float amount);
    void trigger_ability(BossAbility ability);
    BossPhase get_phase() const { return m_phase; }
    float get_health() const { return m_health; }
    float get_shield() const { return m_shield; }
    const char* get_name() const { return m_name; }
    const char* get_ability_log(BossAbility ability) const;
private:
    uint32_t m_entity_id = 0;
    uint32_t m_boss_type = 0;
    float m_health = 500.0f;
    float m_shield = 300.0f;
    float m_armor = 50.0f;
    BossPhase m_phase = BossPhase::Phase1;
    char m_name[48];
    float m_ability_cooldown = 0.0f;
    float m_phase_trigger_health = 350.0f; // phase2
    float m_phase2_trigger_health = 150.0f; // phase3
    float m_buff_timer = 0.0f;
    float m_buff_radius = 12.0f;
};

} // namespace tehi
