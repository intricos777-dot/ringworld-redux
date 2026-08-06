#pragma once
#include <cstdint>

namespace tehi {

enum class InputKey : uint32_t {
    Forward, Back, Left, Right,
    Jump, Crouch, Sprint,
    Fire, AltFire, Reload,
    PrevWeapon, NextWeapon,
    Escape,
};

class PlayerController {
public:
    PlayerController() = default;
    ~PlayerController() = default;

    bool initialize();
    void update(float dt);
    void set_position(float x, float y, float z);
    void set_rotation(float pitch, float yaw, float roll);
    bool is_key_down(InputKey key) const;

    void set_key_state(InputKey key, bool pressed);
    bool wants_fire() const { return m_fire_pressed; }
    bool wants_reload() const { return m_reload_pressed; }
    bool wants_next_weapon() const { return m_next_pressed; }
    bool wants_prev_weapon() const { return m_prev_pressed; }

private:
    uint64_t m_key_state{0};
    mutable bool m_fire_pressed = false;
    mutable bool m_reload_pressed = false;
    mutable bool m_next_pressed = false;
    mutable bool m_prev_pressed = false;
};

} // namespace tehi
