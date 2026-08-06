#include "controller.h"
#include <cstdio>

namespace tehi {

bool PlayerController::initialize() {
    std::printf("[Player] Controller initialized\n");
    return true;
}
void PlayerController::update(float dt) { (void)dt; }
void PlayerController::set_position(float x, float y, float z) {
    std::printf("[Player] Position set to (%f,%f,%f)\n", x, y, z);
}
void PlayerController::set_rotation(float pitch, float yaw, float roll) {
    (void)pitch; (void)yaw; (void)roll;
}
bool PlayerController::is_key_down(InputKey key) const {
    (void)key;
    return false;
}

} // namespace tehi
