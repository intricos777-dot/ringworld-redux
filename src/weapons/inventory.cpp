#include "inventory.h"
#include <cstdio>

namespace tehi {

bool Inventory::add_weapon(uint32_t weapon_id, uint32_t ammo) {
    for (auto& s : m_slots) {
        if (s.weapon_id == weapon_id) {
            s.ammo += ammo;
            return true;
        }
    }
    m_slots.push_back({weapon_id, ammo});
    std::printf("[Inventory] Added weapon %u ammo %u\n", weapon_id, ammo);
    return true;
}
bool Inventory::remove_weapon(uint32_t weapon_id) {
    for (size_t i = 0; i < m_slots.size(); ++i) {
        if (m_slots[i].weapon_id == weapon_id) {
            std::printf("[Inventory] Removed weapon %u\n", weapon_id);
            m_slots.erase(m_slots.begin() + i);
            return true;
        }
    }
    return false;
}
const InventorySlot* Inventory::get_slot(uint32_t index) const {
    if (index >= m_slots.size()) return nullptr;
    return &m_slots[index];
}

} // namespace tehi
