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

class CoopSession {
public:
    CoopSession() = default;
    ~CoopSession() = default;
    bool host(const std::string& mission_path);
    bool join(const std::string& address);
    void add_player(const std::string& name);
    void remove_player(uint32_t slot);
    const std::vector<PlayerSlot>& get_players() const { return m_players; }
    bool is_host() const { return m_host; }
private:
    bool m_host = false;
    std::vector<PlayerSlot> m_players;
    uint32_t m_max_players = 4;
};

} // namespace tehi
