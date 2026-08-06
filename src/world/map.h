#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace tehi {

struct SpawnPoint {
    float position[3];
    float rotation[3];
    uint32_t team = 0;
};

struct Waypoint {
    uint32_t id = 0;
    float position[3];
    float radius = 1.0f;
};

struct Objective {
    uint32_t id = 0;
    std::string description;
    bool completed = false;
};

struct DialogueLine {
    std::string trigger;
    std::string speaker;
    std::string line;
};

class MapData {
public:
    bool load(const std::string& path);
    const std::vector<SpawnPoint>& get_spawns() const { return m_spawns; }
    const std::vector<Waypoint>& get_waypoints() const { return m_waypoints; }
    const std::vector<Objective>& get_objectives() const { return m_objectives; }
    const std::vector<DialogueLine>& get_dialogue() const { return m_dialogue; }
    const std::string& get_map_name() const { return m_map_name; }
    const SpawnPoint& get_player_spawn() const { return m_player_spawn; }
private:
    std::vector<SpawnPoint> m_spawns;
    std::vector<Waypoint> m_waypoints;
    std::vector<Objective> m_objectives;
    std::vector<DialogueLine> m_dialogue;
    std::string m_map_name;
    SpawnPoint m_player_spawn{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0};
};

class Spawner {
public:
    uint32_t spawn_player(float x, float y, float z);
    uint32_t spawn_weapon(uint32_t weapon_type, float x, float y, float z);
    uint32_t spawn_vehicle(uint32_t vehicle_type, float x, float y, float z);
    uint32_t spawn_enemy(float x, float y, float z, uint32_t enemy_type);
};

} // namespace tehi
