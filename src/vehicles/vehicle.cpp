#include "vehicle.h"
#include <cstdio>

namespace tehi {

bool Vehicle::initialize(VehicleType type) {
    m_type = type;
    std::printf("[Vehicle] Initialized type %u\n", (uint32_t)type);
    return true;
}
void Vehicle::update(float dt) { (void)dt; }
void Vehicle::set_driver(uint32_t entity_id) {
    m_driver = entity_id;
    std::printf("[Vehicle] Driver set to entity %u\n", entity_id);
}

} // namespace tehi
