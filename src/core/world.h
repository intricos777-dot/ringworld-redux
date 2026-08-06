#pragma once
#include <vector>
#include <cstdint>

namespace tehi {

struct Entity {
    uint32_t id = 0;
    float position[3] = {0.0f, 0.0f, 0.0f};
    float rotation[3] = {0.0f, 0.0f, 0.0f};
    uint32_t type = 0;
    bool active = true;
};

class World {
public:
    World() = default;
    ~World() = default;
    bool initialize();
    void update(float dt);
    Entity* spawn_entity(uint32_t type, float x, float y, float z);
    std::vector<Entity>& get_entities() { return m_entities; }
private:
    std::vector<Entity> m_entities;
    uint32_t m_next_id = 1;
};

} // namespace tehi
