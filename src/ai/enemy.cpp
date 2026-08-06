#include "enemy.h"
#include <cstdio>

namespace tehi {

bool Enemy::initialize(const EnemyDesc& desc) {
    m_desc = desc;
    m_health = desc.health;
    m_shield = desc.shield;
    std::printf("[Enemy] Spawned %s type=%u hp=%.1f shield=%.1f\n",
        desc.name, (uint32_t)desc.type, m_health, m_shield);
    return true;
}

void Enemy::take_damage(float amount) {
    if (m_shield > 0.0f) {
        m_shield -= amount;
        if (m_shield < 0.0f) {
            m_health += m_shield;
            m_shield = 0.0f;
        }
    } else {
        m_health -= amount;
    }
    if (m_health < 0.0f) m_health = 0.0f;
    std::printf("[Enemy] %s took %.1f damage -> hp=%.1f shield=%.1f\n",
        m_desc.name, amount, m_health, m_shield);
}

void Enemy::update(float dt) {
    (void)dt;
    // stub AI: patrol, chase, attack
}

} // namespace tehi
