#pragma once
#include <cstdint>

namespace tehi {

enum class VehicleType : uint32_t {
    Warthog, Scorpion, Banshee, Ghost, Pelican,
};

class Vehicle {
public:
    Vehicle() = default;
    ~Vehicle() = default;
    bool initialize(VehicleType type);
    void update(float dt);
    void set_driver(uint32_t entity_id);
private:
    VehicleType m_type = VehicleType::Warthog;
    uint32_t m_driver = 0;
};

} // namespace tehi
