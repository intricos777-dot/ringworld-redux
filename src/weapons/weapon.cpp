#include "weapon.h"
#include <cstdio>

namespace tehi {

bool Weapon::initialize(const WeaponDesc& desc) {
    m_desc = desc;
    std::printf("[Weapon] Initialized: %s dmg=%f rate=%f\n", desc.name.c_str(), desc.damage, desc.fire_rate);
    return true;
}
void Weapon::update(float dt) {
    if (m_cooldown > 0.0f) m_cooldown -= dt;
}
void Weapon::fire() {
    if (m_cooldown <= 0.0f) {
        std::printf("[Weapon] Fired %s\n", m_desc.name.c_str());
        m_cooldown = m_desc.fire_rate;
    }
}
void Weapon::reload() {
    std::printf("[Weapon] Reloaded %s\n", m_desc.name.c_str());
}

} // namespace tehi
