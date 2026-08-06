#include "network.h"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

namespace tehi {

static inline double steady_seconds() {
    static const auto origin = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - origin).count();
}

NetworkSystem::NetworkSystem() = default;

NetworkSystem::~NetworkSystem() {
    if (m_socket >= 0) { ::close(m_socket); m_socket = -1; }
    if (m_listen_socket >= 0) { ::close(m_listen_socket); m_listen_socket = -1; }
    for (const auto& c : m_clients) { if (c.tcp_socket >= 0) ::close(c.tcp_socket); }
}

uint64_t NetworkSystem::now_ms() const {
    return (uint64_t)(steady_seconds() * 1000.0);
}

bool NetworkSystem::addr_equal(const sockaddr_in& a, const sockaddr_in& b) {
    return a.sin_family == b.sin_family && a.sin_port == b.sin_port && a.sin_addr.s_addr == b.sin_addr.s_addr;
}

Peer* NetworkSystem::find_peer_by_addr(const sockaddr_in& addr) {
    for (auto& p : m_peers) {
        if (addr_equal(p.addr, addr)) return &p;
    }
    return nullptr;
}

Peer* NetworkSystem::find_peer_by_id(uint32_t id) {
    for (auto& p : m_peers) {
        if (p.id == id) return &p;
    }
    return nullptr;
}

bool NetworkSystem::host(uint16_t port) {
    m_is_host = true;
    m_client_id = 1;
    m_port = port;
    m_peers.clear();
    m_relays.clear();
    m_loss_samples.clear();
    m_interp_buf.clear();
    m_last_state_send_ms = 0;

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
        ::close(m_socket); m_socket = -1;
        return false;
    }
    int opt = 1;
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    m_listen_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_listen_socket >= 0) {
        sockaddr_in tcp_addr{};
        tcp_addr.sin_family = AF_INET;
        tcp_addr.sin_port = htons(port + 1);
        tcp_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (::bind(m_listen_socket, (sockaddr*)&tcp_addr, sizeof(tcp_addr)) == 0) {
            ::listen(m_listen_socket, 16);
            int fl = fcntl(m_listen_socket, F_GETFL, 0);
            fcntl(m_listen_socket, F_SETFL, fl | O_NONBLOCK);
        } else {
            ::close(m_listen_socket); m_listen_socket = -1;
        }
    }

    int fl = fcntl(m_socket, F_GETFL, 0);
    fcntl(m_socket, F_SETFL, fl | O_NONBLOCK);

    std::printf("[Network] Hosting UDP %u / TCP %u\n", port, port + 1);
    return true;
}

bool NetworkSystem::join(const std::string& host, uint16_t port) {
    m_is_host = false;
    m_host = host;
    m_port = port;
    m_client_id = 2;
    m_peers.clear();
    m_relays.clear();
    m_loss_samples.clear();
    m_interp_buf.clear();
    m_last_state_send_ms = 0;

    m_socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socket < 0) {
        std::fprintf(stderr, "[Network] UDP socket failed: %s\n", strerror(errno));
        return false;
    }
    int fl = fcntl(m_socket, F_GETFL, 0);
    fcntl(m_socket, F_SETFL, fl | O_NONBLOCK);

    int tcp = ::socket(AF_INET, SOCK_STREAM, 0);
    if (tcp >= 0) {
        int fl = fcntl(tcp, F_GETFL, 0);
        fcntl(tcp, F_SETFL, fl | O_NONBLOCK);
        sockaddr_in dest{};
        dest.sin_family = AF_INET;
        dest.sin_port = htons(port + 1);
        if (inet_pton(AF_INET, host.c_str(), &dest.sin_addr) == 1) {
            if (::connect(tcp, (sockaddr*)&dest, sizeof(dest)) == 0) {
                ClientEntry c;
                c.id = 2;
                c.ip = ntohl(dest.sin_addr.s_addr);
                c.port = port;
                c.tcp_socket = tcp;
                m_clients.push_back(c);

                Peer peer;
                peer.id = 1;
                sockaddr_in udp_dest = dest;
                udp_dest.sin_port = htons(port);
                peer.addr = udp_dest;
                peer.have_addr = true;
                peer.last_token_time = steady_seconds();
                m_peers.push_back(peer);
                std::printf("[Network] TCP connected to host\n");
            } else {
                ClientEntry c;
                c.id = 2;
                c.ip = ntohl(dest.sin_addr.s_addr);
                c.port = port;
                c.tcp_socket = tcp;
                m_tcp_pending = c;
                std::printf("[Network] TCP connect pending for %s:%u\n", host.c_str(), port);
            }
        } else {
            ::close(tcp);
        }
    }

    const char* hello = "HELLO";
    sockaddr_in dest{};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &dest.sin_addr);
    sendto(m_socket, hello, 5, 0, (sockaddr*)&dest, sizeof(dest));

    if (m_tcp_pending.tcp_socket >= 0) {
        fd_set wf;
        FD_ZERO(&wf);
        FD_SET(m_tcp_pending.tcp_socket, &wf);
        timeval tv{0, 0};
        if (select(m_tcp_pending.tcp_socket + 1, nullptr, &wf, nullptr, &tv) > 0) {
            int err = 0; socklen_t len = sizeof(err);
            if (getsockopt(m_tcp_pending.tcp_socket, SOL_SOCKET, SO_ERROR, &err, &len) == 0 && err == 0) {
                m_clients.push_back(m_tcp_pending);
                Peer peer;
                peer.id = 1;
                peer.addr = dest;
                peer.have_addr = true;
                peer.last_token_time = steady_seconds();
                m_peers.push_back(peer);
                std::printf("[Network] TCP connect completed\n");
            }
            m_tcp_pending = ClientEntry{};
        }
    }

    std::printf("[Network] Joined %s:%u\n", host.c_str(), port);
    return true;
}

void NetworkSystem::update(float dt, const float* entity_positions, uint32_t entity_count) {
    (void)dt;
    if (m_socket < 0 && m_listen_socket < 0) return;

    if (m_is_host && m_listen_socket >= 0) {
        sockaddr_in ca{};
        socklen_t cl = sizeof(ca);
        int tcp = ::accept(m_listen_socket, (sockaddr*)&ca, &cl);
        if (tcp >= 0) {
            int fl = fcntl(tcp, F_GETFL, 0);
            fcntl(tcp, F_SETFL, fl | O_NONBLOCK);
            if (m_clients.size() < 16) {
                ClientEntry c;
                c.id = (uint32_t)(m_clients.size() + 2);
                c.ip = ntohl(ca.sin_addr.s_addr);
                c.port = ntohs(ca.sin_port);
                c.tcp_socket = tcp;
                m_clients.push_back(c);

                Peer peer;
                peer.id = c.id;
                peer.addr = ca;
                peer.have_addr = true;
                peer.last_token_time = steady_seconds();
                m_peers.push_back(peer);

                std::printf("[Network] Client %u TCP connected from %s:%u\n",
                            c.id, inet_ntoa(ca.sin_addr), c.port);
            } else {
                ::close(tcp);
            }
        }
    }

    pump_receive();

    uint64_t now = now_ms();

    for (auto& p : m_peers) {
        if (!p.have_addr) continue;
        if (now - p.last_heartbeat_ms > 500) {
            send_heartbeat(p);
            p.last_heartbeat_ms = now;
        }
    }

    if (m_is_host && entity_positions && entity_count > 0) {
        uint32_t count = entity_count > kNetMaxEntities ? kNetMaxEntities : entity_count;

        float avg_loss = 0.0f;
        if (!m_loss_samples.empty()) {
            for (auto& s : m_loss_samples) avg_loss += s.first;
            avg_loss /= m_loss_samples.size();
        }
        uint64_t interval_ms = 50;
        if (avg_loss > 0.3f) interval_ms = 100;
        if (avg_loss > 0.6f) interval_ms = 200;

        if (now - m_last_state_send_ms > interval_ms) {
            m_last_state_send_ms = now;
            for (auto& p : m_peers) {
                if (!p.have_addr) continue;
                std::vector<uint8_t> payload;
                uint16_t flags = build_entity_state(entity_positions, count, p, payload);
                if (!payload.empty()) {
                    send_frame(p, payload.data(), (uint16_t)payload.size(), kMsgState, flags);
                    send_via_relay(payload.data(), (uint16_t)payload.size(), kMsgState);
                    p.last_state_send_ms = now;
                }
            }
        }
    }

    for (auto& p : m_peers) {
        if (p.pending.empty()) continue;
        auto it = p.pending.begin();
        while (it != p.pending.end()) {
            if (it->size() < kNetHeader) { it = p.pending.erase(it); continue; }
            uint16_t pseq = (uint16_t)((uint16_t)it->at(4) | ((uint16_t)it->at(5) << 8));
            auto st_it = p.send_times.find(pseq);
            if (st_it == p.send_times.end()) { it = p.pending.erase(it); continue; }
            if (now - st_it->second > 400) {
                sendto(m_socket, it->data(), it->size(), 0, (sockaddr*)&p.addr, sizeof(p.addr));
                st_it->second = now;
                p.retrans_packets++;
                p.consecutive_failed_sends = 0;
            }
            ++it;
        }
    }
    for (auto it = m_peers.begin(); it != m_peers.end();) {
        if (it->consecutive_failed_sends > 20) it = m_peers.erase(it);
        else ++it;
    }
}

void NetworkSystem::broadcast_state(const float* state, size_t bytes) {
    if (m_socket < 0 || !state || bytes == 0) return;
    size_t copy = bytes;
    if (copy > 512) copy = 512;
    for (auto& p : m_peers) {
        if (!p.have_addr) continue;
        send_frame(p, (const uint8_t*)state, (uint16_t)copy, kMsgRaw, kFlagReliable);
        send_via_relay((const uint8_t*)state, (uint16_t)copy, kMsgRaw);
    }
}

std::vector<NetPacket> NetworkSystem::receive() {
    std::vector<NetPacket> out;
    pump_receive();
    if (!m_interp_buf.empty()) {
        out.push_back(make_interp_packet());
    }
    return out;
}

void NetworkSystem::set_bandwidth_budget(uint32_t bytes_per_sec) {
    m_budget_bps = bytes_per_sec > 0 ? bytes_per_sec : 4096;
}

bool NetworkSystem::configure_satellite_relay(const std::string& relay_host, uint16_t relay_port) {
    RelayEntry r;
    r.host = relay_host;
    r.port = relay_port;
    r.addr.sin_family = AF_INET;
    r.addr.sin_port = htons(relay_port);
    if (inet_pton(AF_INET, relay_host.c_str(), &r.addr.sin_addr) != 1) {
        r.valid = false;
    } else {
        r.valid = true;
    }
    m_relays.push_back(r);
    std::printf("[Network] Relay configured: %s:%u\n", relay_host.c_str(), relay_port);
    return r.valid;
}

void NetworkSystem::set_interpolation_smoothing(float alpha) {
    m_interp_alpha = alpha;
}

float NetworkSystem::latency_ms() const {
    float sum = 0.0f;
    uint32_t n = 0;
    for (const auto& p : m_peers) { sum += p.rtt_ms; ++n; }
    return n > 0 ? sum / n : 0.0f;
}

float NetworkSystem::packet_loss() const {
    if (m_loss_samples.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& s : m_loss_samples) sum += s.first;
    return sum / m_loss_samples.size();
}

void NetworkSystem::pump_receive() {
    if (m_socket < 0) return;
    uint8_t buf[kNetMtu];
    sockaddr_in from{};
    socklen_t fl = sizeof(from);
    int loops = 0;
    while (loops++ < 32) {
        ssize_t n = recvfrom(m_socket, buf, sizeof(buf), MSG_DONTWAIT, (sockaddr*)&from, &fl);
        if (n <= 0) break;
        if ((size_t)n < kNetHeader) continue;

        Peer* p = nullptr;
        bool from_relay = false;
        for (auto& r : m_relays) {
            if (addr_equal(from, r.addr)) { from_relay = true; break; }
        }

        if (m_is_host) {
            p = find_peer_by_addr(from);
            if (!p && !from_relay) {
                for (auto& existing : m_peers) {
                    if (!existing.have_addr) {
                        existing.addr = from;
                        existing.have_addr = true;
                        existing.last_token_time = steady_seconds();
                        p = &existing;
                        break;
                    }
                }
                if (!p && m_peers.size() < 16) {
                    Peer peer;
                    peer.addr = from;
                    peer.have_addr = true;
                    peer.last_token_time = steady_seconds();
                    m_peers.push_back(peer);
                    p = &m_peers.back();
                }
            }
        } else {
            if (m_peers.empty()) break;
            p = &m_peers[0];
            if (!addr_equal(from, p->addr) && !from_relay) continue;
        }

        if (!p) continue;
        process_datagram(*p, buf, (size_t)n);
    }

    for (auto& p : m_peers) {
        if (!p.have_addr || !p.needs_ack) continue;
        send_frame(p, nullptr, 0, kMsgAck, 0);
        p.needs_ack = false;
    }
}

void NetworkSystem::process_datagram(Peer& p, const uint8_t* data, size_t len) {
    if (len < kNetHeader) return;

    size_t o = 0;
    uint8_t magic = data[o++];
    uint8_t msg_type = data[o++];
    uint16_t flags = (uint16_t)((uint16_t)data[o] | ((uint16_t)data[o + 1] << 8)); o += 2;
    uint16_t seq = (uint16_t)((uint16_t)data[o] | ((uint16_t)data[o + 1] << 8)); o += 2;
    uint16_t ack = (uint16_t)((uint16_t)data[o] | ((uint16_t)data[o + 1] << 8)); o += 2;
    uint16_t ack_bits = (uint16_t)((uint16_t)data[o] | ((uint16_t)data[o + 1] << 8)); o += 2;
    uint32_t from_client = (uint32_t)((uint32_t)data[o] | ((uint32_t)data[o + 1] << 8) |
                                      ((uint32_t)data[o + 2] << 16) | ((uint32_t)data[o + 3] << 24));
    o += 4;
    uint16_t payload_len = (uint16_t)((uint16_t)data[o] | ((uint16_t)data[o + 1] << 8)); o += 2;

    if (magic != kNetMagic || o + payload_len > len) return;

    p.id = from_client;
    p.last_heartbeat_ms = now_ms();

    process_ack(p, ack, ack_bits);

    uint16_t expected = (uint16_t)(p.in_seq + 1);
    if (seq == expected) {
        p.in_seq = seq;
        p.ack_bits = (uint16_t)((p.ack_bits << 1) | 1);
        p.needs_ack = true;
    } else if (seq > expected) {
        uint16_t gap = (uint16_t)(seq - expected);
        if (gap < 16) p.ack_bits |= (uint16_t)(1U << gap);
        p.needs_ack = true;
    } else {
        p.needs_ack = true;
        return;
    }

    if (msg_type == kMsgState && payload_len > 0) {
        NetPacket out;
        if (flags & kFlagFull) {
            apply_full_state(p, data + o, payload_len, out);
        } else {
            apply_delta(p, data + o, payload_len, out);
        }
        m_interp_buf.push_back({out, now_ms()});
        if (m_interp_buf.size() > 32) m_interp_buf.pop_front();
    } else if (msg_type == kMsgRaw && payload_len > 0) {
        NetPacket out;
        std::memset(&out, 0, sizeof(out));
        out.from_client = from_client;
        out.type = 2;
        size_t copy = payload_len;
        if (copy > sizeof(out.data)) copy = sizeof(out.data);
        std::memcpy(out.data, data + o, copy);
        m_interp_buf.push_back({out, now_ms()});
        if (m_interp_buf.size() > 32) m_interp_buf.pop_front();
    }
}

bool NetworkSystem::send_frame(Peer& p, const uint8_t* payload, uint16_t payload_len, uint8_t msg_type, uint16_t flags) {
    if (m_socket < 0 || !p.have_addr) return false;
    size_t total = kNetHeader + payload_len;
    if (total > kNetMtu) return false;
    if (!consume_tokens(p, total)) return false;

    uint8_t buf[kNetMtu];
    size_t o = 0;
    buf[o++] = kNetMagic;
    buf[o++] = msg_type;
    buf[o++] = (flags >> 0) & 0xFF;
    buf[o++] = (flags >> 8) & 0xFF;
    uint16_t seq = p.out_seq++;
    buf[o++] = (seq >> 0) & 0xFF;
    buf[o++] = (seq >> 8) & 0xFF;
    uint16_t ack = p.in_seq;
    buf[o++] = (ack >> 0) & 0xFF;
    buf[o++] = (ack >> 8) & 0xFF;
    uint16_t abits = p.ack_bits;
    buf[o++] = (abits >> 0) & 0xFF;
    buf[o++] = (abits >> 8) & 0xFF;
    uint32_t fc = htonl(p.id);
    std::memcpy(buf + o, &fc, 4); o += 4;
    uint16_t pl = htons(payload_len);
    std::memcpy(buf + o, &pl, 2); o += 2;
    if (payload && payload_len > 0) {
        std::memcpy(buf + o, payload, payload_len);
        o += payload_len;
    }

    sendto(m_socket, buf, o, 0, (sockaddr*)&p.addr, sizeof(p.addr));
    if (errno == EWOULDBLOCK || errno == ECONNREFUSED) {
        p.consecutive_failed_sends++;
        if (p.consecutive_failed_sends > 20) std::printf("[Network] Peer %u send failures, will drop\n", p.id);
    }
    p.sent_packets++;
    p.send_times[seq] = now_ms();

    if (msg_type == kMsgState || msg_type == kMsgRaw) {
        std::vector<uint8_t> frame(buf, buf + o);
        p.pending.push_back(std::move(frame));
        if (p.pending.size() > 64) p.pending.erase(p.pending.begin());
    }

    return true;
}

void NetworkSystem::send_heartbeat(Peer& p) {
    send_frame(p, nullptr, 0, kMsgAck, kFlagHeartbeat);
}

void NetworkSystem::send_via_relay(const uint8_t* payload, uint16_t len, uint8_t msg_type) {
    if (m_relays.empty() || m_socket < 0) return;
    uint8_t wrap[kNetMtu];
    if (6 + len > sizeof(wrap)) return;
    size_t o = 0;
    uint32_t cid = htonl(m_client_id);
    std::memcpy(wrap + o, &cid, 4); o += 4;
    wrap[o++] = msg_type;
    uint16_t fl = htons(0);
    std::memcpy(wrap + o, &fl, 2); o += 2;
    uint16_t pl = htons(len);
    std::memcpy(wrap + o, &pl, 2); o += 2;
    std::memcpy(wrap + o, payload, len); o += len;
    for (auto& r : m_relays) {
        if (!r.valid) continue;
        sendto(m_socket, wrap, o, 0, (sockaddr*)&r.addr, sizeof(r.addr));
    }
}

void NetworkSystem::apply_delta(Peer& p, const uint8_t* payload, uint16_t len, NetPacket& out) {
    if (len < 4) return;
    uint32_t count = (uint32_t)payload[0] | ((uint32_t)payload[1] << 8) |
                     ((uint32_t)payload[2] << 16) | ((uint32_t)payload[3] << 24);
    if (count > kNetMaxEntities) count = kNetMaxEntities;
    size_t o = 4;
    for (uint32_t i = 0; i < count && o + 1 <= len; ++i) {
        uint8_t mask = payload[o++];
        for (int c = 0; c < 3; ++c) {
            if ((mask & (1 << c)) && o + 2 <= len) {
                int16_t qd = (int16_t)((uint16_t)payload[o] | ((uint16_t)payload[o + 1] << 8));
                o += 2;
                p.ref_state[i * 3 + c] += qd * kDeltaInvScale;
            }
        }
    }
    p.ref_count = count;

    std::memset(&out, 0, sizeof(out));
    out.from_client = p.id;
    out.type = 1;
    std::memcpy(&out.data[0], &count, 4);
    for (uint32_t i = 0; i < count; ++i) {
        std::memcpy(&out.data[1 + i * 3 + 0], &p.ref_state[i * 3 + 0], 4);
        std::memcpy(&out.data[1 + i * 3 + 1], &p.ref_state[i * 3 + 1], 4);
        std::memcpy(&out.data[1 + i * 3 + 2], &p.ref_state[i * 3 + 2], 4);
    }
}

void NetworkSystem::apply_full_state(Peer& p, const uint8_t* payload, uint16_t len, NetPacket& out) {
    if (len < 4) return;
    uint32_t count = (uint32_t)payload[0] | ((uint32_t)payload[1] << 8) |
                     ((uint32_t)payload[2] << 16) | ((uint32_t)payload[3] << 24);
    if (count > kNetMaxEntities) count = kNetMaxEntities;
    size_t needed = 4 + (size_t)count * 12;
    if (len < needed) return;

    const uint8_t* pos = payload + 4;
    for (uint32_t i = 0; i < count; ++i) {
        for (int c = 0; c < 3; ++c) {
            float v;
            std::memcpy(&v, pos, 4);
            pos += 4;
            p.ref_state[i * 3 + c] = v;
        }
    }
    p.ref_count = count;

    std::memset(&out, 0, sizeof(out));
    out.from_client = p.id;
    out.type = 1;
    std::memcpy(&out.data[0], &count, 4);
    for (uint32_t i = 0; i < count; ++i) {
        std::memcpy(&out.data[1 + i * 3 + 0], &p.ref_state[i * 3 + 0], 4);
        std::memcpy(&out.data[1 + i * 3 + 1], &p.ref_state[i * 3 + 1], 4);
        std::memcpy(&out.data[1 + i * 3 + 2], &p.ref_state[i * 3 + 2], 4);
    }
}

bool NetworkSystem::consume_tokens(Peer& p, size_t bytes) {
    refill_tokens(p);
    if (p.token_bytes >= bytes) {
        p.token_bytes -= (uint32_t)bytes;
        return true;
    }
    return false;
}

void NetworkSystem::refill_tokens(Peer& p) {
    double now = steady_seconds();
    if (p.last_token_time == 0.0) {
        p.last_token_time = now;
        p.token_bytes = m_budget_bps;
        return;
    }
    double dt = now - p.last_token_time;
    if (dt > 0.0) {
        uint64_t add = (uint64_t)((double)m_budget_bps * dt);
        p.token_bytes += add;
        if (p.token_bytes > m_budget_bps * 2) p.token_bytes = m_budget_bps * 2;
    }
    p.last_token_time = now;
}

uint16_t NetworkSystem::build_entity_state(const float* positions, uint32_t count, Peer& p, std::vector<uint8_t>& out) {
    bool can_delta = (count > 0 && p.ref_count == count && p.ref_seq == (uint16_t)(p.out_seq - 1) && p.out_seq > 1);
    uint16_t flags = can_delta ? kFlagDelta : kFlagFull;

    out.clear();
    out.reserve(4 + (can_delta ? count * 4 : count * 12));
    out.push_back(count & 0xFF);
    out.push_back((count >> 8) & 0xFF);
    out.push_back((count >> 16) & 0xFF);
    out.push_back((count >> 24) & 0xFF);

    if (!can_delta || count == 0) {
        for (uint32_t i = 0; i < count; ++i) {
            for (int c = 0; c < 3; ++c) {
                uint32_t bits;
                std::memcpy(&bits, &positions[i * 3 + c], 4);
                out.push_back(bits & 0xFF);
                out.push_back((bits >> 8) & 0xFF);
                out.push_back((bits >> 16) & 0xFF);
                out.push_back((bits >> 24) & 0xFF);
            }
        }
        if (count > 0) {
            p.ref_count = count;
            std::memcpy(p.ref_state, positions, count * 12);
        }
    } else {
        for (uint32_t i = 0; i < count; ++i) {
            uint8_t mask = 0;
            int16_t qd[3] = {0, 0, 0};
            for (int c = 0; c < 3; ++c) {
                float delta = positions[i * 3 + c] - p.ref_state[i * 3 + c];
                if (std::fabs(delta) >= kDeltaThreshold) {
                    mask |= (1 << c);
                    qd[c] = (int16_t)std::round(delta * kDeltaScale);
                }
            }
            out.push_back(mask);
            if (mask & 1) { out.push_back(qd[0] & 0xFF); out.push_back((qd[0] >> 8) & 0xFF); }
            if (mask & 2) { out.push_back(qd[1] & 0xFF); out.push_back((qd[1] >> 8) & 0xFF); }
            if (mask & 4) { out.push_back(qd[2] & 0xFF); out.push_back((qd[2] >> 8) & 0xFF); }
            for (int c = 0; c < 3; ++c) {
                if (mask & (1 << c)) p.ref_state[i * 3 + c] = positions[i * 3 + c];
            }
        }
        p.ref_seq = (uint16_t)(p.out_seq - 1);
    }

    return flags;
}

NetPacket NetworkSystem::make_interp_packet() const {
    NetPacket out;
    std::memset(&out, 0, sizeof(out));
    if (m_interp_buf.empty()) return out;
    const auto& snap = m_interp_buf.back();
    out = snap.first;
    return out;
}

void NetworkSystem::process_ack(Peer& p, uint16_t ack_seq, uint16_t ack_bits) {
    if (ack_seq == 0) return;

    auto st_it = p.send_times.find(ack_seq);
    if (st_it != p.send_times.end()) {
        uint64_t now = now_ms();
        float rtt = (float)(now - st_it->second);
        if (p.rtt_ms > 0.0f) p.rtt_ms = p.rtt_ms * 0.8f + rtt * 0.2f;
        else p.rtt_ms = rtt;
        p.send_times.erase(st_it);
    }

    auto pit = p.pending.begin();
    while (pit != p.pending.end()) {
        if (pit->size() < kNetHeader) { pit = p.pending.erase(pit); continue; }
        uint16_t pseq = (uint16_t)((uint16_t)pit->at(4) | ((uint16_t)pit->at(5) << 8));
        bool acked = false;
        if (pseq == ack_seq) acked = true;
        else if (ack_seq > pseq && (uint16_t)(ack_seq - pseq) <= 16) {
            uint16_t bit = (uint16_t)(ack_seq - pseq - 1);
            if (ack_bits & (1U << bit)) acked = true;
        }
        if (acked) {
            p.acked_packets++;
            pit = p.pending.erase(pit);
        } else {
            ++pit;
        }
    }

    if (p.sent_packets > 0) {
        p.loss_ratio = 1.0f - (float)p.acked_packets / (float)p.sent_packets;
        m_loss_samples.push_back({p.loss_ratio, now_ms()});
        if (m_loss_samples.size() > 64) m_loss_samples.pop_front();
    }
}

} // namespace tehi
