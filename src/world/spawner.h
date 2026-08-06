#pragma once
#include <cstdint>

namespace tehi {

class Spawner {
public:
    uint32_t spawn_player(float x, float y, float z);
    uint32_t spawn_weapon(uint32_t weapon_type, float x, float y, float z);
    uint32_t spawn_vehicle(uint32_t vehicle_type, float x, float y, float z);
};

} // namespace tehi
