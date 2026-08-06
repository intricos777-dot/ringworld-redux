#include "weapon.h"
#include "weapon_registry.h"
#include <cstdio>

namespace tehi {

bool Weapon::initialize(RealWeaponID id) {
    m_real_id = id;
    const auto* spec = get_weapon_spec(id);
    if (!spec) return false;

    m_damage = spec->damage;
    m_fire_rate = spec->fire_rate;
    m_mag_size = spec->mag_size;
    m_reserve = spec->reserve_max;
    m_ammo_in_mag = m_mag_size;
    m_cooldown = 0.0f;
    m_reloading = 0.0f;

    std::printf("[Weapon] Initialized: %s (%s) dmg=%.1f rate=%.3f mag=%u reserve=%u\n",
        spec->display_name,
        spec->real_world_counterpart,
        m_damage,
        m_fire_rate,
        m_mag_size,
        m_reserve);
    return true;
}

void Weapon::update(float dt) {
    if (m_cooldown > 0.0f) m_cooldown -= dt;
    if (m_reloading > 0.0f) {
        m_reloading -= dt;
        if (m_reloading <= 0.0f) {
            uint32_t needed = m_mag_size - m_ammo_in_mag;
            uint32_t take = (needed < m_reserve) ? needed : m_reserve;
            m_reserve -= take;
            m_ammo_in_mag += take;
            std::printf("[Weapon] Reload complete: %u / %u\n", m_ammo_in_mag, m_reserve);
        }
    }
}

void Weapon::fire() {
    if (m_cooldown > 0.0f || m_reloading > 0.0f || m_ammo_in_mag == 0) return;

    const auto* spec = get_weapon_spec(m_real_id);
    if (!spec) return;

    m_ammo_in_mag--;
    m_cooldown = m_fire_rate;
    std::printf("[Weapon] Fired %s ammo=%u/%u dmg=%.1f\n",
        spec->display_name, m_ammo_in_mag, m_reserve, m_damage);
}

void Weapon::reload() {
    if (m_reloading > 0.0f) return;
    if (m_ammo_in_mag == m_mag_size) return;
    if (m_reserve == 0) return;

    m_reloading = 1.8f;
    std::printf("[Weapon] Reloading...\n");
}

const char* Weapon::get_display_name() const {
    const auto* spec = get_weapon_spec(m_real_id);
    if (!spec) return "(unknown)";
    return spec->display_name;
}

} // namespace tehi
