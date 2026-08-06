#include "spawner.h"
#include <cstdio>

namespace tehi {

uint32_t Spawner::spawn_player(float x, float y, float z) {
    std::printf("[World] Spawn player at (%f,%f,%f)\n", x, y, z);
    return 1;
}
uint32_t Spawner::spawn_weapon(uint32_t weapon_type, float x, float y, float z) {
    std::printf("[World] Spawn weapon %u at (%f,%f,%f)\n", weapon_type, x, y, z);
    return 1;
}
uint32_t Spawner::spawn_vehicle(uint32_t vehicle_type, float x, float y, float z) {
    std::printf("[World] Spawn vehicle %u at (%f,%f,%f)\n", vehicle_type, x, y, z);
    return 1;
}

} // namespace tehi
