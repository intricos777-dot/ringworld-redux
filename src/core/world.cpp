#include "world.h"
#include <cstdio>

namespace tehi {

bool World::initialize() {
    std::printf("[World] Initialized with 0 entities\n");
    return true;
}

void World::update(float dt) {
    (void)dt;
    for (auto& e : m_entities) {
        if (!e.active) continue;
        // stub update
    }
}

Entity* World::spawn_entity(uint32_t type, float x, float y, float z) {
    Entity e;
    e.id = m_next_id++;
    e.type = type;
    e.position[0] = x; e.position[1] = y; e.position[2] = z;
    e.active = true;
    m_entities.push_back(e);
    std::printf("[World] Spawned entity %u type %u at (%f,%f,%f)\n", e.id, type, x, y, z);
    return &m_entities.back();
}

} // namespace tehi
