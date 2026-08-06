#include "coop.h"
#include <cstdio>

namespace tehi {

bool CoopSession::host(const std::string& mission_path) {
    m_host = true;
    std::printf("[Coop] Hosting mission: %s\n", mission_path.c_str());
    add_player("Player1");
    return true;
}

bool CoopSession::join(const std::string& address) {
    m_host = false;
    std::printf("[Coop] Joining session at: %s\n", address.c_str());
    return true;
}

void CoopSession::add_player(const std::string& name) {
    if (m_players.size() >= m_max_players) {
        std::printf("[Coop] Session full\n");
        return;
    }
    PlayerSlot slot;
    slot.entity_id = (uint32_t)(m_players.size() + 1);
    slot.name = name;
    slot.ready = true;
    slot.team = 0;
    m_players.push_back(slot);
    std::printf("[Coop] Player joined: %s (slot %u)\n", name.c_str(), slot.entity_id);
}

void CoopSession::remove_player(uint32_t slot) {
    for (size_t i = 0; i < m_players.size(); ++i) {
        if (m_players[i].entity_id == slot) {
            std::printf("[Coop] Player left: %s\n", m_players[i].name.c_str());
            m_players.erase(m_players.begin() + i);
            return;
        }
    }
}

} // namespace tehi
