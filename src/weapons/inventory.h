#pragma once
#include <cstdint>
#include <string>
#include <array>
#include "weapon_registry.h"

namespace tehi {

enum class Slot : uint32_t {
    Main = 0,
    Secondary,
    Space,
    COUNT
};

struct InventorySlot {
    uint32_t weapon_id = 0;
    uint32_t ammo = 0;
};

class Inventory {
public:
    Inventory() = default;
    ~Inventory() = default;

    bool equip(RealWeaponID id);
    bool unequip(Slot slot);
    const InventorySlot* get_slot(Slot slot) const { return &m_slots[(uint32_t)slot]; }
    InventorySlot* get_slot(Slot slot) { return &m_slots[(uint32_t)slot]; }
    void update(float dt);

private:
    std::array<InventorySlot, (uint32_t)Slot::COUNT> m_slots{};
};

} // namespace tehi
