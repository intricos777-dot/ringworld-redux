#include "camera.h"
#include <cstdio>

namespace tehi {

bool Camera::initialize() {
    std::printf("[Camera] Initialized\n");
    return true;
}
void Camera::update(float dt) { (void)dt; }
void Camera::set_view(float pitch, float yaw, float roll) {
    std::printf("[Camera] View set pitch=%f yaw=%f roll=%f\n", pitch, yaw, roll);
}

} // namespace tehi
