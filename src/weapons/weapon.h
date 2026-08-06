#pragma once
#include <cstdint>
#include <string>

namespace tehi {

enum class WeaponType : uint32_t {
    Pistol, Rifle, Shotgun, Sniper, Rocket, Energy,
    Melee, Grenade,
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
    bool initialize(const WeaponDesc& desc);
    void update(float dt);
    void fire();
    void reload();
private:
    WeaponDesc m_desc;
    float m_cooldown = 0.0f;
};

} // namespace tehi
