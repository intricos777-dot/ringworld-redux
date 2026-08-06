#pragma once
#include <cstdint>
#include <vector>
#include <string>

namespace tehi {

class World;

enum class AIState : uint32_t {
    Idle,
    Hold,
    Advance,
    Flank,
    Suppress,
    Retreat,
    Attack
};

enum class TacticalRole : uint32_t {
    Rifleman,
    AutomaticRifleman,
    Grenadier,
    Marksman,
    Medic,
    SquadLeader
};

struct AITacticalOrder {
    AIState state;
    float target_x;
    float target_y;
    float target_z;
    uint32_t target_entity;
    float priority;
};

class AIAgent {
public:
    AIAgent() = default;
    ~AIAgent() = default;

    bool initialize(uint32_t entity_id, TacticalRole role);
    void update(float dt, World* world);
    void issue_order(const AITacticalOrder& order);
    void set_state(AIState state);

    uint32_t get_entity_id() const { return m_entity_id; }
    AIState get_state() const { return m_state; }
    TacticalRole get_role() const { return m_role; }
    float get_health() const { return m_health; }

private:
    uint32_t m_entity_id = 0;
    TacticalRole m_role = TacticalRole::Rifleman;
    AIState m_state = AIState::Idle;
    float m_health = 100.0f;
    float m_state_timer = 0.0f;
    AITacticalOrder m_current_order;
};

class AISquad {
public:
    AISquad() = default;
    ~AISquad() = default;

    bool initialize(uint32_t leader_entity);
    void update(float dt, World* world);
    void issue_squad_order(AIState state, float x, float y, float z);

    void add_agent(uint32_t entity_id, TacticalRole role);
    const std::vector<AIAgent>& get_agents() const { return m_agents; }
    uint32_t get_leader_id() const { return m_leader_id; }

private:
    std::vector<AIAgent> m_agents;
    uint32_t m_leader_id = 0;
    AIState m_squad_state = AIState::Hold;
};

} // namespace tehi
