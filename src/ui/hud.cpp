#include "hud.h"
#include <cstdio>
#include <cstring>

namespace tehi {

static float g_health = 100.0f;
static float g_shield = 100.0f;
static float g_ammo = 32.0f;
static float g_reserve = 128.0f;
static bool g_initialized = false;

bool HUD::initialize() {
    std::printf("[HUD] System online. Rendering tactical overlay.\n");
    g_initialized = true;
    return true;
}

void HUD::draw_crosshair() {
    std::printf("[HUD] [ + ]\n");
}

void HUD::draw_health(float health) {
    g_health = health;
}

void HUD::draw_shield(float shield) {
    g_shield = shield;
}

void HUD::draw_ammo(uint32_t current, uint32_t reserve) {
    g_ammo = (float)current;
    g_reserve = (float)reserve;
}

void HUD::render() const {
    if (!g_initialized) return;
    std::printf("[HUD] ------------------------------------------------\n");
    std::printf("[HUD] HEALTH : [%s", g_health > 60 ? "||||||||||" : g_health > 30 ? "||||||...." : "||........");
    std::printf("] %.1f\n", g_health);
    std::printf("[HUD] SHIELD : [%s", g_shield > 60 ? "||||||||||" : g_shield > 30 ? "||||||...." : "||........");
    std::printf("] %.1f\n", g_shield);
    std::printf("[HUD] AMMO   : %03.0f / %03.0f\n", g_ammo, g_reserve);
    std::printf("[HUD] ------------------------------------------------\n");
}

} // namespace tehi
