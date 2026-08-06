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

class NetworkSystem {
public:
    NetworkSystem();
    ~NetworkSystem();
    bool host(uint16_t port);
    bool join(const std::string& host, uint16_t port);
    void update(float dt);
    void broadcast_state(const float* state, size_t bytes);
    std::vector<NetPacket> receive();
    uint32_t client_id() const { return m_client_id; }
    bool is_host() const { return m_is_host; }
private:
    bool m_is_host = false;
    uint32_t m_client_id = 0;
    int m_socket = -1;
    std::string m_host;
    uint16_t m_port = 0;
};

} // namespace tehi
