#pragma once
#include <vector>
#include <cstdint>

namespace tehi {

struct InventorySlot {
    uint32_t weapon_id = 0;
    uint32_t ammo = 0;
};

class Inventory {
public:
    Inventory() = default;
    ~Inventory() = default;
    bool add_weapon(uint32_t weapon_id, uint32_t ammo);
    bool remove_weapon(uint32_t weapon_id);
    const InventorySlot* get_slot(uint32_t index) const;
    size_t size() const { return m_slots.size(); }
private:
    std::vector<InventorySlot> m_slots;
};

} // namespace tehi
