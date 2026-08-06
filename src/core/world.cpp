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
    e.health = 100.0f;
    e.shield = 0.0f;
    if (type == 1) { e.health = 100.0f; }
    else if (type == 4) { e.health = 80.0f; }
    m_entities.push_back(e);
    std::printf("[World] Spawned entity %u type %u at (%f,%f,%f)\n", e.id, type, x, y, z);
    return &m_entities.back();
}

void World::damage_entity(uint32_t entity_id, float amount) {
    for (auto& e : m_entities) {
        if (e.id == entity_id && e.active) {
            if (e.shield > 0.0f) {
                e.shield -= amount;
                if (e.shield < 0.0f) {
                    e.health += e.shield;
                    e.shield = 0.0f;
                }
            } else {
                e.health -= amount;
            }
            if (e.health < 0.0f) {
                e.health = 0.0f;
                e.active = false;
                std::printf("[World] Entity %u destroyed\n", entity_id);
            } else {
                std::printf("[World] Entity %u took %.1f damage -> hp=%.1f shield=%.1f\n",
                    entity_id, amount, e.health, e.shield);
            }
            return;
        }
    }
    std::printf("[World] Entity %u not found\n", entity_id);
}

} // namespace tehi
