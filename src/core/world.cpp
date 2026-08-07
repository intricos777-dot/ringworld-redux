#include "world.h"
#include "core/alien_panic.h"
#include "core/game.h"
#include "campaign/mission.h"
#include <cstdio>
#include <algorithm>
#include <cctype>

namespace tehi {

World::World() = default;
World::~World() = default;

bool World::initialize() {
    m_alien_panic.initialize();
    std::printf("[World] Initialized with panic system\n");
    return true;
}

void World::register_alien(uint32_t entity_id, uint32_t team, uint32_t rank, uint32_t squad_id) {
    m_alien_panic.add_alien(entity_id,
        rank == 2 ? AIAlienRank::Elite : AIAlienRank::Grunt,
        squad_id);
}

void World::notify_elite_killed(uint32_t entity_id, uint32_t squad_id) {
    m_alien_panic.notify_elite_killed(squad_id);
}

void World::update(float dt) {
    m_alien_panic.update(dt);
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
                // Find squad_id for this entity and notify panic system
                for (const auto& alien : m_alien_panic.get_aliens()) {
                    if (alien.entity_id == entity_id) {
                        notify_elite_killed(entity_id, alien.squad_id);
                        break;
                    }
                }
                // Check kill-based objectives
                auto* game = Game::instance_ptr();
                if (game && game->get_mission()) {
                    const char* type_name = nullptr;
                    switch (e.type) {
                        case 1: type_name = "drone"; break;
                        case 2: type_name = "target"; break;
                        case 3: type_name = "seeker"; break;
                        case 4: type_name = "commander"; break;
                        default: break;
                    }
                    if (type_name) {
                        for (auto& obj : game->get_mission()->get_objectives()) {
                            if (!obj.completed) {
                                std::string desc = obj.description;
                                std::transform(desc.begin(), desc.end(), desc.begin(), ::tolower);
                                if (desc.find(type_name) != std::string::npos) {
                                    game->get_mission()->complete_objective(obj.id);
                                }
                            }
                        }
                    }
                }
            } else {
                std::printf("[World] Entity %u took %.1f damage -> hp=%.1f shield=%.1f\n",
                    entity_id, amount, e.health, e.shield);
            }
            return;
        }
    }
    std::printf("[World] Entity %u not found\n", entity_id);
}

void World::heal_entity(uint32_t entity_id, float amount) {
    for (auto& e : m_entities) {
        if (e.id == entity_id && e.active) {
            e.health += amount;
            if (e.health > 120.0f) e.health = 120.0f;
            return;
        }
    }
}

} // namespace tehi
