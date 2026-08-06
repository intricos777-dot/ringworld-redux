#include "network.h"
#include <cstdio>
#include <cstring>

namespace tehi {

NetworkSystem::NetworkSystem() = default;
NetworkSystem::~NetworkSystem() = default;

bool NetworkSystem::host(uint16_t port) {
    m_is_host = true;
    m_client_id = 1;
    m_port = port;
    std::printf("[Network] Hosting on port %u\n", port);
    return true;
}

bool NetworkSystem::join(const std::string& host, uint16_t port) {
    m_is_host = false;
    m_host = host;
    m_port = port;
    m_client_id = 2;
    std::printf("[Network] Joining %s:%u\n", host.c_str(), port);
    return true;
}

void NetworkSystem::update(float dt) {
    (void)dt;
    if (m_socket < 0) return;
}

void NetworkSystem::broadcast_state(const float* state, size_t bytes) {
    (void)state;
    (void)bytes;
    if (m_socket < 0) return;
}

std::vector<NetPacket> NetworkSystem::receive() {
    std::vector<NetPacket> out;
    if (m_socket < 0) return out;
    return out;
}

} // namespace tehi
