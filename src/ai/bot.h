#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace tehi {

struct NavPoint {
    float position[3];
    float radius = 1.0f;
};

class NavMesh {
public:
    NavMesh() = default;
    ~NavMesh() = default;
    bool load(const std::string& path);
    const NavPoint* find_nearest(float x, float y, float z) const;
    const std::vector<NavPoint>& get_points() const { return m_points; }
private:
    std::vector<NavPoint> m_points;
};

class Bot {
public:
    Bot() = default;
    ~Bot() = default;
    bool initialize(uint32_t entity_id);
    void update(float dt);
private:
    uint32_t m_entity_id = 0;
    float m_state_timer = 0.0f;
};

} // namespace tehi
