#include "network.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace tehi {

NetworkSystem::NetworkSystem() = default;
NetworkSystem::~NetworkSystem() {
    if (m_socket >= 0) {
        ::close(m_socket);
        m_socket = -1;
    }
}

bool NetworkSystem::host(uint16_t port) {
    m_is_host = true;
    m_client_id = 1;
    m_port = port;
    m_socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socket < 0) {
        std::fprintf(stderr, "[Network] socket failed: %s\n", strerror(errno));
        return false;
    }
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (::bind(m_socket, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::fprintf(stderr, "[Network] bind failed: %s\n", strerror(errno));
        ::close(m_socket);
        m_socket = -1;
        return false;
    }
    std::printf("[Network] Hosting on port %u\n", port);
    return true;
}

bool NetworkSystem::join(const std::string& host, uint16_t port) {
    m_is_host = false;
    m_host = host;
    m_port = port;
    m_client_id = 2;
    m_socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socket < 0) {
        std::fprintf(stderr, "[Network] socket failed: %s\n", strerror(errno));
        return false;
    }
    sockaddr_in dest{};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &dest.sin_addr) != 1) {
        std::fprintf(stderr, "[Network] bad host address: %s\n", host.c_str());
        ::close(m_socket);
        m_socket = -1;
        return false;
    }
    const char* hello = "HELLO";
    sendto(m_socket, hello, 5, 0, (sockaddr*)&dest, sizeof(dest));
    std::printf("[Network] Joining %s:%u\n", host.c_str(), port);
    return true;
}

void NetworkSystem::update(float dt, const float* entity_positions, uint32_t entity_count) {
    (void)dt;
    if (m_socket < 0) return;
    char buf[256];
    sockaddr_in from{};
    socklen_t fromlen = sizeof(from);
    ssize_t n = recvfrom(m_socket, buf, sizeof(buf) - 1, MSG_DONTWAIT, (sockaddr*)&from, &fromlen);
    if (n > 0) {
        buf[n] = 0;
        if (m_is_host) {
            bool known = false;
            uint32_t ip = ntohl(from.sin_addr.s_addr);
            uint16_t port = ntohs(from.sin_port);
            for (const auto& c : m_clients) {
                if (c.ip == ip && c.port == port) { known = true; break; }
            }
            if (!known && m_clients.size() < 16) {
                ClientEntry c;
                c.id = (uint32_t)(m_clients.size() + 2);
                c.ip = ip;
                c.port = port;
                m_clients.push_back(c);
                std::printf("[Network] Client %u connected from %s:%u\n", c.id, inet_ntoa(from.sin_addr), port);
            }
        }
        std::printf("[Network] recv %zd bytes from %s\n", n, inet_ntoa(from.sin_addr));
    }
    if (m_is_host && entity_positions && entity_count > 0) {
        std::printf("[Network] broadcasting %u entities\n", entity_count);
        uint32_t count = entity_count > 12 ? 12 : entity_count;
        size_t bytes = sizeof(uint32_t) + count * 3 * sizeof(float);
        if (bytes > sizeof(buf)) bytes = sizeof(buf);
        std::memcpy(buf, &count, sizeof(count));
        std::memcpy(buf + sizeof(count), entity_positions, bytes - sizeof(count));
        sockaddr_in dest{};
        dest.sin_family = AF_INET;
        dest.sin_port = htons(m_port);
        dest.sin_addr.s_addr = htonl(INADDR_BROADCAST);
        int opt = 1;
        setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));
        sendto(m_socket, buf, bytes, 0, (sockaddr*)&dest, sizeof(dest));
    }
}

void NetworkSystem::broadcast_state(const float* state, size_t bytes) {
    if (m_socket < 0 || !state || bytes == 0) return;
    sockaddr_in dest{};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(m_port);
    if (m_is_host) {
        dest.sin_addr.s_addr = htonl(INADDR_BROADCAST);
        int opt = 1;
        setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));
    } else {
        if (inet_pton(AF_INET, m_host.c_str(), &dest.sin_addr) != 1) return;
    }
    sendto(m_socket, (const char*)state, bytes, 0, (sockaddr*)&dest, sizeof(dest));
}

std::vector<NetPacket> NetworkSystem::receive() {
    std::vector<NetPacket> out;
    if (m_socket < 0) return out;
    char buf[256];
    sockaddr_in from{};
    socklen_t fromlen = sizeof(from);
    ssize_t n = recvfrom(m_socket, buf, sizeof(buf), MSG_DONTWAIT, (sockaddr*)&from, &fromlen);
    if (n >= (ssize_t)sizeof(NetPacket)) {
        NetPacket pkt;
        std::memcpy(&pkt, buf, sizeof(pkt));
        out.push_back(pkt);
    } else if (n > 0 && m_is_host) {
        // hello or other text
    }
    return out;
}

} // namespace tehi
