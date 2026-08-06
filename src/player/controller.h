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
};

} // namespace tehi
