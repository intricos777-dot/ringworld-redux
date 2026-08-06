#include "inventory.h"
#include "weapon.h"
#include "weapon_registry.h"
#include <cstdio>

namespace tehi {

bool Inventory::equip(RealWeaponID id) {
    const auto* spec = get_weapon_spec(id);
    if (!spec) return false;

    Slot target = Slot::Main;
    if (spec->type == (uint32_t)WeaponType::Pistol) target = Slot::Secondary;
    else if (spec->type == (uint32_t)WeaponType::Grenade) target = Slot::Secondary;
    else if (spec->type == (uint32_t)WeaponType::Energy || spec->type == (uint32_t)WeaponType::Rocket) target = Slot::Space;

    auto& slot = m_slots[(uint32_t)target];
    slot.weapon_id = (uint32_t)id;
    slot.ammo = spec->reserve_max;
    std::printf("[Inventory] Equipped %s in slot=%u reserve=%u\n", spec->display_name, (uint32_t)target, slot.ammo);
    return true;
}

bool Inventory::unequip(Slot slot) {
    auto& s = m_slots[(uint32_t)slot];
    if (s.weapon_id == 0) return false;
    std::printf("[Inventory] Unequipped slot=%u\n", (uint32_t)slot);
    s.weapon_id = 0;
    s.ammo = 0;
    return true;
}

void Inventory::update(float dt) {
    (void)dt;
    // TODO: ammo regen, durability, etc.
}

} // namespace tehi
