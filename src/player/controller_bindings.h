#pragma once
#include <cstdint>
#include <array>

namespace tehi {

enum class ControllerType : uint32_t {
    Keyboard,
    Xbox360,
    XboxOne,
    XboxSeries,
    PS3,
    PS4,
    PS5,
    GenericGamepad
};

struct ControllerBinding {
    ControllerType type;
    uint32_t button_mask;
    uint32_t axis_threshold;
    bool swap_thumbsticks;
    bool invert_left_y;
    bool invert_right_y;
    float deadzone;
};

constexpr std::array<ControllerBinding, 8> DEFAULT_BINDINGS{{
    {ControllerType::Keyboard,    0xFFFFFFFF, 16384, false, false, false, 0.05f},
    {ControllerType::Xbox360,     0xFFFFFFFF, 16384, false, false, false, 0.08f},
    {ControllerType::XboxOne,     0xFFFFFFFF, 16384, false, false, false, 0.08f},
    {ControllerType::XboxSeries,  0xFFFFFFFF, 16384, false, false, false, 0.08f},
    {ControllerType::PS3,         0xFFFFFFFF, 16384, true,  false, false, 0.10f},
    {ControllerType::PS4,         0xFFFFFFFF, 16384, false, false, true,  0.08f},
    {ControllerType::PS5,         0xFFFFFFFF, 16384, false, false, true,  0.08f},
    {ControllerType::GenericGamepad,0xFFFFFFFF, 16384, false, false, false, 0.10f},
}};

} // namespace tehi
