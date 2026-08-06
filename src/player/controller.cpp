#include "controller.h"
#include <cstdio>

namespace tehi {

bool PlayerController::initialize() {
    std::printf("[Player] Controller initialized\n");
    return true;
}

void PlayerController::update(float dt) {
    (void)dt;
    // In a real implementation, this would read from the input system.
    // For now, we just expose query methods for the game loop.
}

void PlayerController::set_position(float x, float y, float z) {
    std::printf("[Player] Position set to (%f,%f,%f)\n", x, y, z);
}

void PlayerController::set_rotation(float pitch, float yaw, float roll) {
    (void)pitch; (void)yaw; (void)roll;
}

bool PlayerController::is_key_down(InputKey key) const {
    uint64_t mask = 1ULL << static_cast<uint32_t>(key);
    return (m_key_state & mask) != 0;
}

void PlayerController::set_key_state(InputKey key, bool pressed) {
    uint64_t mask = 1ULL << static_cast<uint32_t>(key);
    if (pressed) {
        m_key_state |= mask;
    } else {
        m_key_state &= ~mask;
    }
    if (key == InputKey::Fire) m_fire_pressed = pressed;
    if (key == InputKey::Reload) m_reload_pressed = pressed;
    if (key == InputKey::NextWeapon) m_next_pressed = pressed;
    if (key == InputKey::PrevWeapon) m_prev_pressed = pressed;
}

} // namespace tehi
