#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace tehi {

struct NetPacket {
    uint32_t from_client;
    uint32_t type;
    float data[8];
};

struct ClientEntry {
    uint32_t id;
    uint32_t ip;
    uint16_t port;
};

class NetworkSystem {
public:
    NetworkSystem();
    ~NetworkSystem();
    bool host(uint16_t port);
    bool join(const std::string& host, uint16_t port);
    void update(float dt, const float* entity_positions = nullptr, uint32_t entity_count = 0);
    void broadcast_state(const float* state, size_t bytes);
    std::vector<NetPacket> receive();
    uint32_t client_id() const { return m_client_id; }
    bool is_host() const { return m_is_host; }
    uint32_t connected_client_count() const { return (uint32_t)m_clients.size(); }
private:
    bool m_is_host = false;
    uint32_t m_client_id = 0;
    int m_socket = -1;
    std::string m_host;
    uint16_t m_port = 0;
    std::vector<ClientEntry> m_clients;
};

} // namespace tehi
