#pragma once
#include <cstdint>
#include <string>
#include "weapon_registry.h"

namespace tehi {

enum class WeaponType : uint32_t {
    Pistol = 0,
    Rifle,
    Shotgun,
    Sniper,
    Rocket,
    Energy,
    Melee,
    Grenade,
};

struct WeaponDesc {
    WeaponType type;
    std::string name;
    float damage = 10.0f;
    float fire_rate = 0.2f;
    uint32_t ammo = 20;
};

class Weapon {
public:
    Weapon() = default;
    ~Weapon() = default;

    bool initialize(RealWeaponID id);
    void update(float dt);
    void fire();
    void reload();

    RealWeaponID get_real_id() const { return m_real_id; }
    const char* get_display_name() const;
    float get_damage() const { return m_damage; }
    float get_fire_rate() const { return m_fire_rate; }
    uint32_t get_mag_size() const { return m_mag_size; }
    uint32_t get_reserve() const { return m_reserve; }
    uint32_t get_ammo_in_mag() const { return m_ammo_in_mag; }

private:
    RealWeaponID m_real_id = RealWeaponID::COUNT;
    float m_damage = 10.0f;
    float m_fire_rate = 0.2f;
    uint32_t m_mag_size = 20;
    uint32_t m_reserve = 0;
    uint32_t m_ammo_in_mag = 0;
    float m_cooldown = 0.0f;
    float m_reloading = 0.0f;
};

} // namespace tehi
