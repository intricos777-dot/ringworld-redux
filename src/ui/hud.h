#pragma once
#include <cstdint>

namespace tehi {

class HUD {
public:
    HUD() = default;
    ~HUD() = default;
    bool initialize();
    void draw_crosshair();
    void draw_health(float health);
    void draw_shield(float shield);
    void draw_ammo(uint32_t current, uint32_t reserve);
};

} // namespace tehi
