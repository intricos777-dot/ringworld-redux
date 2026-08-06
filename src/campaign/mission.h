#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace tehi {

struct MissionObjective {
    uint32_t id = 0;
    std::string description;
    bool completed = false;
};

struct Checkpoint {
    float position[3];
    float rotation[3];
};

struct DialogueLine {
    std::string trigger;
    std::string speaker;
    std::string line;
};

struct EnemyEntry {
    std::string type;
    uint32_t count = 0;
    float position[3];
};

struct HiddenSpawn {
    uint32_t id = 0;
    float position[3];
    std::string enemy_type;
    uint32_t count = 0;
};

struct Waypoint {
    uint32_t id = 0;
    float position[3];
    float radius = 0.0f;
};

class Mission {
public:
    Mission() = default;
    ~Mission() = default;
    bool initialize(const std::string& path);
    bool update(float dt, const float* player_position);
    void save_checkpoint();
    bool load_checkpoint();
    void complete_objective(uint32_t id);
    void trigger_dialogue(const std::string& trigger);
    const std::vector<MissionObjective>& get_objectives() const { return m_objectives; }
    const std::vector<DialogueLine>& get_dialogue() const { return m_dialogue; }
    const std::vector<EnemyEntry>& get_enemies() const { return m_enemies; }
    const std::vector<HiddenSpawn>& get_hidden_spawns() const { return m_hidden_spawns; }
    const std::vector<Waypoint>& get_waypoints() const { return m_waypoints; }
    bool is_finale() const { return m_finale; }
    std::string id() const { return m_id; }
    std::string name() const { return m_name; }
    std::string description() const { return m_description; }
    std::string map() const { return m_map; }
private:
    std::string m_id;
    std::string m_name;
    std::string m_description;
    std::string m_map;
    std::vector<MissionObjective> m_objectives;
    std::vector<Checkpoint> m_checkpoints;
    std::vector<DialogueLine> m_dialogue;
    std::vector<EnemyEntry> m_enemies;
    std::vector<HiddenSpawn> m_hidden_spawns;
    std::vector<Waypoint> m_waypoints;
    uint32_t m_current_checkpoint = 0;
    bool m_finale = false;
    uint32_t m_last_printed_objective_id = 0;
    bool m_on_spawn_printed = false;
};

} // namespace tehi
