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

class Map {
public:
    Map() = default;
    ~Map() = default;
    bool load(const std::string& path);
    const std::vector<SpawnPoint>& get_spawns() const { return m_spawns; }
private:
    std::vector<SpawnPoint> m_spawns;
};

class Spawner {
public:
    Spawner() = default;
    ~Spawner() = default;
    uint32_t spawn_player(float x, float y, float z);
    uint32_t spawn_weapon(uint32_t weapon_type, float x, float y, float z);
    uint32_t spawn_vehicle(uint32_t vehicle_type, float x, float y, float z);
};

} // namespace tehi
