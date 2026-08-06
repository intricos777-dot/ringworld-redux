#include "network.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

namespace tehi {

NetworkSystem::NetworkSystem() = default;
NetworkSystem::~NetworkSystem() {
    if (m_socket >= 0) {
        ::close(m_socket);
        m_socket = -1;
    }
    if (m_listen_socket >= 0) {
        ::close(m_listen_socket);
        m_listen_socket = -1;
    }
    for (const auto& c : m_clients) {
        if (c.tcp_socket >= 0) ::close(c.tcp_socket);
    }
}

bool NetworkSystem::host(uint16_t port) {
    m_is_host = true;
    m_client_id = 1;
    m_port = port;
    
    // UDP socket for broadcast state
    m_socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socket < 0) {
        std::fprintf(stderr, "[Network] UDP socket failed: %s\n", strerror(errno));
        return false;
    }
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (::bind(m_socket, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::fprintf(stderr, "[Network] UDP bind failed: %s\n", strerror(errno));
        ::close(m_socket);
        m_socket = -1;
        return false;
    }
    
    // TCP listen socket for client connections
    m_listen_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_listen_socket < 0) {
        std::fprintf(stderr, "[Network] TCP listen socket failed: %s\n", strerror(errno));
        return false;
    }
    int opt = 1;
    setsockopt(m_listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sockaddr_in tcp_addr{};
    tcp_addr.sin_family = AF_INET;
    tcp_addr.sin_port = htons(port + 1);
    tcp_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (::bind(m_listen_socket, (sockaddr*)&tcp_addr, sizeof(tcp_addr)) < 0) {
        std::fprintf(stderr, "[Network] TCP bind failed: %s\n", strerror(errno));
        ::close(m_listen_socket);
        m_listen_socket = -1;
        return false;
    }
    if (::listen(m_listen_socket, 16) < 0) {
        std::fprintf(stderr, "[Network] TCP listen failed: %s\n", strerror(errno));
        ::close(m_listen_socket);
        m_listen_socket = -1;
        return false;
    }
    // Set non-blocking
    int flags = fcntl(m_listen_socket, F_GETFL, 0);
    fcntl(m_listen_socket, F_SETFL, flags | O_NONBLOCK);
    flags = fcntl(m_socket, F_GETFL, 0);
    fcntl(m_socket, F_SETFL, flags | O_NONBLOCK);
    
    std::printf("[Network] Hosting UDP %u / TCP %u\n", port, port + 1);
    return true;
}

bool NetworkSystem::join(const std::string& host, uint16_t port) {
    m_is_host = false;
    m_host = host;
    m_port = port;
    m_client_id = 2;
    
    // UDP socket
    m_socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socket < 0) {
        std::fprintf(stderr, "[Network] UDP socket failed: %s\n", strerror(errno));
        return false;
    }
    
    // TCP connect to host
    int tcp = ::socket(AF_INET, SOCK_STREAM, 0);
    if (tcp < 0) {
        std::fprintf(stderr, "[Network] TCP socket failed: %s\n", strerror(errno));
        return false;
    }
    sockaddr_in dest{};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port + 1);
    if (inet_pton(AF_INET, host.c_str(), &dest.sin_addr) != 1) {
        std::fprintf(stderr, "[Network] bad host address: %s\n", host.c_str());
        ::close(tcp);
        return false;
    }
    if (::connect(tcp, (sockaddr*)&dest, sizeof(dest)) < 0) {
        std::fprintf(stderr, "[Network] TCP connect failed: %s\n", strerror(errno));
        ::close(tcp);
        return false;
    }
    ClientEntry c;
    c.id = 2;
    c.ip = ntohl(dest.sin_addr.s_addr);
    c.port = port;
    c.tcp_socket = tcp;
    m_clients.push_back(c);
    
    const char* hello = "HELLO";
    sendto(m_socket, hello, 5, 0, (sockaddr*)&dest, sizeof(dest));
    std::printf("[Network] Joined %s:%u (TCP connected)\n", host.c_str(), port);
    return true;
}

void NetworkSystem::update(float dt, const float* entity_positions, uint32_t entity_count) {
    (void)dt;
    if (m_socket < 0 && m_listen_socket < 0) return;
    
    // Accept new TCP connections (host only)
    if (m_is_host && m_listen_socket >= 0) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int tcp = ::accept(m_listen_socket, (sockaddr*)&client_addr, &client_len);
        if (tcp >= 0) {
            int flags = fcntl(tcp, F_GETFL, 0);
            fcntl(tcp, F_SETFL, flags | O_NONBLOCK);
            if (m_clients.size() < 16) {
                ClientEntry c;
                c.id = (uint32_t)(m_clients.size() + 2);
                c.ip = ntohl(client_addr.sin_addr.s_addr);
                c.port = ntohs(client_addr.sin_port);
                c.tcp_socket = tcp;
                m_clients.push_back(c);
                std::printf("[Network] Client %u TCP connected from %s:%u\n", c.id, inet_ntoa(client_addr.sin_addr), c.port);
            } else {
                ::close(tcp);
            }
        }
    }
    
    // UDP receive
    if (m_socket >= 0) {
        char buf[256];
        sockaddr_in from{};
        socklen_t fromlen = sizeof(from);
        ssize_t n = recvfrom(m_socket, buf, sizeof(buf) - 1, MSG_DONTWAIT, (sockaddr*)&from, &fromlen);
        if (n > 0) {
            buf[n] = 0;
            if (!m_is_host) {
                std::printf("[Network] recv %zd bytes from host\n", n);
            }
        }
    }
    
    // Host: broadcast entity state via UDP
    if (m_is_host && entity_positions && entity_count > 0 && m_socket >= 0) {
        uint32_t count = entity_count > 12 ? 12 : entity_count;
        size_t bytes = sizeof(uint32_t) + count * 3 * sizeof(float);
        char buf[256];
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
    }
    return out;
}

} // namespace tehi
