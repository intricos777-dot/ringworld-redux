#include "boss_alien.h"
#include "core/world.h"
#include <cstdio>
#include <cstring>

namespace tehi {

static const BossTechProfile BOSS_PROFILES[] = {
    {"Oracle Warden", BossAbility::ShieldDrainPulse, BossAbility::SummonMinions, BossAbility::EnergyBeam, 55.0f, 12.0f,
     "A segmented command unit with adaptive shielding and bonded sentry swarms."},
    {"Gravity Tyrant", BossAbility::HeavyArmor, BossAbility::EnergyBeam, BossAbility::EMPBurst, 75.0f, 8.0f,
     "Massive quadruped chassis with gravitic plating and localized weapon jamming."},
    {"Phantom Prime", BossAbility::CloakPhase, BossAbility::SentryDeploy, BossAbility::EnergyBeam, 45.0f, 10.0f,
     "Stealth command frame that phases between realities and deploys seeker drones."},
};

BossAlien::BossAlien() = default;
BossAlien::~BossAlien() = default;

bool BossAlien::initialize(uint32_t entity_id, uint32_t boss_type) {
    m_entity_id = entity_id;
    m_boss_type = boss_type;
    if (boss_type >= sizeof(BOSS_PROFILES) / sizeof(BOSS_PROFILES[0])) {
        m_boss_type = 0;
    }
    const auto& profile = BOSS_PROFILES[m_boss_type];
    m_health = 500.0f;
    m_shield = 300.0f;
    m_armor = profile.armor_rating;
    m_phase = BossPhase::Phase1;
    m_ability_cooldown = 0.0f;
    std::snprintf(m_name, sizeof(m_name), "%s", profile.name);
    std::printf("[BossAlien] Initialized %s entity=%u health=%.1f shield=%.1f armor=%.1f\n",
        m_name, m_entity_id, m_health, m_shield, m_armor);
    return true;
}

void BossAlien::update(float dt, World* world) {
    if (m_phase == BossPhase::Defeated) return;
    if (m_ability_cooldown > 0.0f) m_ability_cooldown -= dt;

    if (m_health <= m_phase2_trigger_health && m_phase == BossPhase::Phase2) {
        m_phase = BossPhase::Phase3;
        std::printf("[BossAlien] %s entered Phase 3\n", m_name);
    } else if (m_health <= m_phase_trigger_health && m_phase == BossPhase::Phase1) {
        m_phase = BossPhase::Phase2;
        std::printf("[BossAlien] %s entered Phase 2\n", m_name);
    }

    if (m_health <= 0.0f) {
        m_phase = BossPhase::Defeated;
        std::printf("[BossAlien] %s defeated\n", m_name);
        return;
    }

    if (world) {
        m_buff_timer += dt;
        if (m_buff_timer >= 1.0f) {
            m_buff_timer = 0.0f;
            float bx = 0.0f, bz = 0.0f;
            for (const auto& e : world->get_entities()) {
                if (e.id == m_entity_id) {
                    bx = e.position[0];
                    bz = e.position[2];
                    break;
                }
            }
            for (const auto& e : world->get_entities()) {
                if (!e.active || e.id == m_entity_id) continue;
                float dx = e.position[0] - bx;
                float dz = e.position[2] - bz;
                float dist = dx*dx + dz*dz;
                if (dist < m_buff_radius * m_buff_radius) {
                    world->heal_entity(e.id, 2.0f);
                }
            }
            std::printf("[BossAlien] %s buff pulse\n", m_name);
        }
    }
}

void BossAlien::take_damage(float amount) {
    if (m_phase == BossPhase::Defeated) return;
    float absorbed = amount - m_armor;
    if (absorbed < 0.0f) absorbed = 0.0f;
    if (m_shield > 0.0f) {
        m_shield -= absorbed;
        if (m_shield < 0.0f) {
            m_health += m_shield;
            m_shield = 0.0f;
        }
    } else {
        m_health -= absorbed;
    }
    if (m_health < 0.0f) m_health = 0.0f;
    std::printf("[BossAlien] %s took %.1f damage -> hp=%.1f shield=%.1f armor=%.1f\n",
        m_name, absorbed, m_health, m_shield, m_armor);
}

void BossAlien::trigger_ability(BossAbility ability) {
    if (m_ability_cooldown > 0.0f) return;
    std::printf("[BossAlien] %s used %s\n", m_name, get_ability_log(ability));
    m_ability_cooldown = 4.0f;
}

const char* BossAlien::get_ability_log(BossAbility ability) const {
    switch (ability) {
        case BossAbility::ShieldDrainPulse: return "Shield Drain Pulse";
        case BossAbility::SummonMinions: return "Summon Minions";
        case BossAbility::HeavyArmor: return "Heavy Armor";
        case BossAbility::EnergyBeam: return "Energy Beam";
        case BossAbility::EMPBurst: return "EMP Burst";
        case BossAbility::CloakPhase: return "Cloak/Phase";
        case BossAbility::SentryDeploy: return "Sentry Deploy";
        default: return "None";
    }
}

} // namespace tehi
