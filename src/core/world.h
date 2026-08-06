#pragma once
#include <vector>
#include <cstdint>
#include "alien_panic.h"

namespace tehi {

struct Entity {
    uint32_t id = 0;
    float position[3] = {0.0f, 0.0f, 0.0f};
    float rotation[3] = {0.0f, 0.0f, 0.0f};
    uint32_t type = 0;
    bool active = true;
    float health = 100.0f;
    float shield = 0.0f;
};

class World {
public:
    World();
    ~World();
    bool initialize();
    void update(float dt);
    Entity* spawn_entity(uint32_t type, float x, float y, float z);
    void damage_entity(uint32_t entity_id, float amount);
    void heal_entity(uint32_t entity_id, float amount);
    void register_alien(uint32_t entity_id, uint32_t team, uint32_t rank, uint32_t squad_id);
    void notify_elite_killed(uint32_t entity_id, uint32_t squad_id);
    std::vector<Entity>& get_entities() { return m_entities; }
private:
    std::vector<Entity> m_entities;
    uint32_t m_next_id = 1;
    AlienSquadManager m_alien_panic;
};

} // namespace tehi
