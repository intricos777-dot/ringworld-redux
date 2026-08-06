#include "hud.h"
#include <cstdio>

namespace tehi {

bool HUD::initialize() {
    std::printf("[HUD] Initialized\n");
    return true;
}
void HUD::draw_crosshair() {
    std::printf("[HUD] Crosshair\n");
}
void HUD::draw_health(float health) {
    std::printf("[HUD] Health: %f\n", health);
}
void HUD::draw_shield(float shield) {
    std::printf("[HUD] Shield: %f\n", shield);
}
void HUD::draw_ammo(uint32_t current, uint32_t reserve) {
    std::printf("[HUD] Ammo: %u / %u\n", current, reserve);
}

} // namespace tehi
