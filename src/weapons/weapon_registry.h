#pragma once
#include <cstdint>
#include <string>
#include <array>

namespace tehi {

// Real-world US military weapon definitions, tuned for game balance.
// All ballistic data is theoretical/game-fidelity, not live ballistics.

enum class RealWeaponID : uint32_t {
    // Conventional
    M1911_45ACP = 0,
    M17_9MM,
    M4A1_CARBINE,
    M16A4_RIFLE,
    M110_SASS,
    M40A5_SNIPER,
    M249_SAW,
    M1014_SHOTGUN,
    M67_FRAG,
    M136_AT4,
    MP7A1,
    M9A1_PISTOL,
    // Theoretical space weapons
    PLASMA_RIFLE,
    PARTICLE_CANNON,
    GAUSS_GUN,
    LASER_PISTOL,
    RAILGUN,
    COUNT
};

struct RealWeaponSpec {
    RealWeaponID id;
    uint32_t type; // matches WeaponType enum values: 0=Pistol,1=Rifle,2=Shotgun,3=Sniper,4=Rocket,5=Energy,6=Melee,7=Grenade
    const char* display_name;
    const char* real_world_counterpart;
    float damage;
    float fire_rate;       // seconds between shots
    uint32_t mag_size;
    uint32_t reserve_max;
    float muzzle_velocity; // m/s, theoretical
    float effective_range; // meters, theoretical
    float weight;          // kg, theoretical
    uint32_t armor_pierce; // 0=none, 1=light, 2=medium, 3=heavy
};

inline constexpr std::array<RealWeaponSpec, (uint32_t)RealWeaponID::COUNT> REAL_WEAPONS{{
    // Conventional US military firearms
    { RealWeaponID::M1911_45ACP,     0, "M1911 .45 ACP",        "Colt M1911 / MEU(SOC)",         28.0f, 0.25f, 7,  42,  260.0f, 50.0f,  1.1f, 1},
    { RealWeaponID::M17_9MM,         0, "M17 9mm",              "SIG Sauer P320 (M17/M18)",       22.0f, 0.18f, 17, 68,  360.0f, 50.0f,  0.85f,1},
    { RealWeaponID::M4A1_CARBINE,    1, "M4A1",                 "Colt M4A1 Carbine",              35.0f, 0.09f, 30, 120, 880.0f, 550.0f,  2.9f, 2},
    { RealWeaponID::M16A4_RIFLE,     1, "M16A4",                "Colt M16A4 Rifle",               34.0f, 0.10f, 30, 120, 950.0f, 600.0f,  3.2f, 2},
    { RealWeaponID::M110_SASS,       3, "M110",                 "Knight's Armament SR-25",        65.0f, 0.9f,  20, 80,  780.0f, 800.0f,  6.9f, 3},
    { RealWeaponID::M40A5_SNIPER,    3, "M40A5",                "Remington M40A5 Sniper",         72.0f, 1.1f,  5,  25,  800.0f,1000.0f, 6.6f, 3},
    { RealWeaponID::M249_SAW,        1, "M249 SAW",             "FN M249 Squad Automatic Weapon", 32.0f, 0.08f, 200,600, 915.0f, 700.0f, 7.5f, 2},
    { RealWeaponID::M1014_SHOTGUN,   2, "M1014",                "Benelli M1014 Super 90",         45.0f, 0.45f, 5,  35,  400.0f, 70.0f,  3.8f, 1},
    { RealWeaponID::M67_FRAG,        7, "M67 Fragmentation",    "M67 Hand Grenade",               90.0f, 1.0f,  1,  4,    0.0f, 30.0f,  0.4f, 2},
    { RealWeaponID::M136_AT4,        4, "M136 AT4",             "AT4 Anti-Tank Weapon",          120.0f, 1.5f,  1,  2,  295.0f, 300.0f, 6.7f, 3},
    { RealWeaponID::MP7A1,           1, "MP7A1",                "Heckler & Koch MP7A1",           24.0f, 0.07f, 40, 160, 700.0f, 200.0f, 1.8f, 1},
    { RealWeaponID::M9A1_PISTOL,     0, "M9A1",                 "Beretta M9A1",                   20.0f, 0.20f, 15, 60,  390.0f, 50.0f,  0.97f,1},
    // Theoretical space weapons
    { RealWeaponID::PLASMA_RIFLE,    5, "XPR-1 Plasma Rifle",   "Theoretical EXO-Plasma Rifle",   55.0f, 0.12f, 35, 140, 1200.0f, 350.0f, 4.2f, 2},
    { RealWeaponID::PARTICLE_CANNON, 5, "PC-9 Particle Cannon", "Theoretical Particle Accelerator",95.0f, 0.6f,  5,  20, 1800.0f, 900.0f, 9.5f, 3},
    { RealWeaponID::GAUSS_GUN,       1, "GG-7 Gauss Gun",       "Theoretical Magnetic Coil Gun",  70.0f, 0.35f, 12, 48, 2400.0f, 1100.0f,5.4f, 3},
    { RealWeaponID::LASER_PISTOL,    0, "LP-2 Laser Pistol",    "Theoretical Directed Energy Sidearm",18.0f,0.08f, 50,200, 299792.0f,120.0f,0.65f,1},
    { RealWeaponID::RAILGUN,         3, "RG-X Railgun",         "Theoretical Electromagnetic Railgun",150.0f,2.5f, 3, 12, 4200.0f, 2200.0f,12.0f, 3},
}};

inline const RealWeaponSpec* get_weapon_spec(RealWeaponID id) {
    for (const auto& w : REAL_WEAPONS) {
        if (w.id == id) return &w;
    }
    return nullptr;
}

} // namespace tehi
