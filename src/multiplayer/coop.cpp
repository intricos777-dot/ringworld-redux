#include "coop.h"
#include <cstdio>
#include <algorithm>

namespace tehi {

CoopSession::CoopSession() = default;
CoopSession::~CoopSession() = default;

bool CoopSession::host(const std::string& mission_path, SessionMode mode) {
    m_host = true;
    m_mode = mode;
    std::printf("[Coop] Hosting %s | mode=%s | max=%u\n",
        mission_path.c_str(),
        mode == SessionMode::LocalServer ? "local server" : "peer-to-peer",
        m_max_players);
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
        std::printf("[Coop] Session full (%u/%u)\n", (uint32_t)m_players.size(), m_max_players);
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

std::vector<ServerEntry> CoopSession::discover_local_servers() const {
    std::vector<ServerEntry> entries;
    ServerEntry e;
    e.name = "LocalHost";
    e.address = "127.0.0.1";
    e.players = (uint32_t)m_players.size();
    e.max_players = m_max_players;
    e.mode = SessionMode::LocalServer;
    entries.push_back(e);
    return entries;
}

} // namespace tehi
