#pragma once
#include <cstdint>
#include "controller_bindings.h"

namespace tehi {

enum class InputKey : uint32_t {
    Forward, Back, Left, Right,
    Jump, Crouch, Sprint,
    Fire, AltFire, Reload,
    PrevWeapon, NextWeapon,
    Escape,
    COUNT
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

    void set_controller_type(ControllerType type);
    ControllerType get_controller_type() const { return m_controller_type; }
    const ControllerBinding& get_binding() const;

private:
    uint64_t m_key_state{0};
    ControllerType m_controller_type{ControllerType::Keyboard};
    mutable bool m_fire_pressed = false;
    mutable bool m_reload_pressed = false;
    mutable bool m_next_pressed = false;
    mutable bool m_prev_pressed = false;
};

inline const ControllerBinding& PlayerController::get_binding() const {
    for (const auto& b : DEFAULT_BINDINGS) {
        if (b.type == m_controller_type) return b;
    }
    return DEFAULT_BINDINGS[0];
}

} // namespace tehi