#pragma once
#include <cstdint>
#include <vector>
#include <string>

namespace tehi {

struct PlayerSlot {
    uint32_t entity_id = 0;
    std::string name;
    bool ready = false;
    uint32_t team = 0;
};

enum class SessionMode : uint32_t {
    LocalServer,
    PeerToPeer
};

struct ServerEntry {
    std::string name;
    std::string address;
    uint32_t players = 0;
    uint32_t max_players = 16;
    SessionMode mode = SessionMode::LocalServer;
};

class CoopSession {
public:
    CoopSession();
    ~CoopSession();
    bool host(const std::string& mission_path, SessionMode mode);
    bool join(const std::string& address);
    void add_player(const std::string& name);
    void remove_player(uint32_t slot);
    const std::vector<PlayerSlot>& get_players() const { return m_players; }
    bool is_host() const { return m_host; }
    uint32_t max_players() const { return m_max_players; }
    std::vector<ServerEntry> discover_local_servers() const;
private:
    bool m_host = false;
    std::vector<PlayerSlot> m_players;
    uint32_t m_max_players = 16;
    SessionMode m_mode = SessionMode::LocalServer;
};

} // namespace tehi
