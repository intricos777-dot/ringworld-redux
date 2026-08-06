#include "squad.h"
#include "core/world.h"
#include <cstdio>

namespace tehi {

bool AIAgent::initialize(uint32_t entity_id, TacticalRole role) {
    m_entity_id = entity_id;
    m_role = role;
    m_health = 100.0f;
    m_state = AIState::Idle;
    m_state_timer = 0.0f;
    std::printf("[AI] Agent entity=%u role=%u initialized\n", entity_id, (uint32_t)role);
    return true;
}

void AIAgent::set_state(AIState state) {
    if (m_state != state) {
        m_state = state;
        m_state_timer = 0.0f;
    }
}

void AIAgent::issue_order(const AITacticalOrder& order) {
    m_current_order = order;
    set_state(order.state);
    std::printf("[AI] Agent %u ordered: state=%u target=(%.1f,%.1f,%.1f)\n",
        m_entity_id, (uint32_t)order.state, order.target_x, order.target_y, order.target_z);
}

void AIAgent::update(float dt, World* world) {
    m_state_timer += dt;
    if (!world) return;

    auto& entities = world->get_entities();
    for (auto& e : entities) {
        if (e.id == m_entity_id) {
            switch (m_state) {
                case AIState::Advance:
                    e.position[0] += dt * 2.0f;
                    break;
                case AIState::Flank:
                    e.position[2] += dt * 1.5f;
                    break;
                case AIState::Retreat:
                    e.position[0] -= dt * 3.0f;
                    break;
                case AIState::Suppress:
                case AIState::Attack:
                    if (m_state_timer > 1.0f) {
                        std::printf("[AI] Agent %u firing at target %u\n", m_entity_id, m_current_order.target_entity);
                        m_state_timer = 0.0f;
                    }
                    break;
                default:
                    break;
            }
            break;
        }
    }
}

bool AISquad::initialize(uint32_t leader_entity) {
    m_leader_id = leader_entity;
    m_agents.clear();
    m_squad_state = AIState::Hold;
    std::printf("[AI] Squad initialized leader=%u\n", leader_entity);
    return true;
}

void AISquad::add_agent(uint32_t entity_id, TacticalRole role) {
    AIAgent agent;
    agent.initialize(entity_id, role);
    m_agents.push_back(agent);
}

void AISquad::issue_squad_order(AIState state, float x, float y, float z) {
    m_squad_state = state;
    AITacticalOrder order{state, x, y, z, 0, 1.0f};
    for (auto& agent : m_agents) {
        agent.issue_order(order);
    }
    const char* state_name = state == AIState::Advance ? "Advance" :
                             state == AIState::Flank ? "Flank" :
                             state == AIState::Suppress ? "Suppress" :
                             state == AIState::Retreat ? "Retreat" :
                             state == AIState::Attack ? "Attack" : "Hold";
    std::printf("[AI] Squad order: %s to (%.1f,%.1f,%.1f)\n", state_name, x, y, z);
}

void AISquad::update(float dt, World* world) {
    for (auto& agent : m_agents) {
        agent.update(dt, world);
    }
}

} // namespace tehi
