#pragma once
#include <cstdint>

namespace tehi {

class Camera {
public:
    Camera() = default;
    ~Camera() = default;
    bool initialize();
    void update(float dt);
    void set_view(float pitch, float yaw, float roll);
};

} // namespace tehi
