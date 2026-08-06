#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <chrono>
#include <netinet/in.h>

namespace tehi {

struct NetPacket {
    uint32_t from_client;
    uint32_t type;
    float data[37];
};

constexpr uint32_t kNetMaxClients = 16;
constexpr uint32_t kNetMaxEntities = 12;
constexpr uint16_t kNetMtu = 1400;
constexpr uint16_t kNetHeader = 16;
constexpr uint8_t kNetMagic = 0x52;
constexpr uint16_t kFlagFull = 0x01;
constexpr uint16_t kFlagDelta = 0x02;
constexpr uint16_t kFlagReliable = 0x04;
constexpr uint16_t kFlagHeartbeat = 0x08;
constexpr uint8_t kMsgAck = 0;
constexpr uint8_t kMsgState = 1;
constexpr uint8_t kMsgRaw = 2;
constexpr uint16_t kDeltaScale = 200;
constexpr float kDeltaInvScale = 1.0f / 200.0f;
constexpr float kDeltaThreshold = 0.02f;

struct ClientEntry {
    uint32_t id;
    uint32_t ip;
    uint16_t port;
    int tcp_socket;
    uint8_t active = 0;
    uint64_t last_heard_ms = 0;
    float rtt_ms = 0.0f;
    uint32_t addr_u32 = 0;
    uint16_t addr_port = 0;
    uint64_t last_acked_seq = 0;
    uint32_t last_snap_count = 0;
    float last_snap[36] = {};
};

struct Peer {
    uint32_t id = 0;
    sockaddr_in addr{};
    bool have_addr = false;
    uint16_t out_seq = 1;
    uint16_t in_seq = 0;
    uint16_t ack_bits = 0;
    bool needs_ack = false;
    uint64_t last_heartbeat_ms = 0;
    uint64_t last_state_send_ms = 0;
    float rtt_ms = 0.0f;
    float loss_ratio = 0.0f;
    uint32_t token_bytes = 0;
    double last_token_time = 0.0;
    std::vector<std::vector<uint8_t>> pending;
    std::map<uint16_t, uint64_t> send_times;
    uint32_t ref_count = 0;
    float ref_state[36] = {};
    uint16_t ref_seq = 0;
    uint64_t sent_packets = 0;
    uint64_t acked_packets = 0;
    uint64_t retrans_packets = 0;
};

struct RelayEntry {
    std::string host;
    uint16_t port;
    sockaddr_in addr{};
    bool valid = false;
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

    void set_bandwidth_budget(uint32_t bytes_per_sec);
    bool configure_satellite_relay(const std::string& relay_host, uint16_t relay_port);
    void set_interpolation_smoothing(float alpha);
    float latency_ms() const;
    float packet_loss() const;

private:
    bool m_is_host = false;
    uint32_t m_client_id = 0;
    int m_socket = -1;
    int m_listen_socket = -1;
    std::string m_host;
    uint16_t m_port = 0;
    std::vector<ClientEntry> m_clients;
    std::vector<Peer> m_peers;
    std::vector<RelayEntry> m_relays;
    std::deque<std::pair<float, uint64_t>> m_loss_samples;
    std::deque<std::pair<NetPacket, uint64_t>> m_interp_buf;
    float m_interp_alpha = 0.2f;
    uint32_t m_budget_bps = 4096;
    uint64_t m_last_state_send_ms = 0;

    uint64_t now_ms() const;
    Peer* find_peer_by_addr(const sockaddr_in& addr);
    Peer* find_peer_by_id(uint32_t id);
    void pump_receive();
    void process_datagram(Peer& p, const uint8_t* data, size_t len);
    bool send_frame(Peer& p, const uint8_t* payload, uint16_t payload_len, uint8_t msg_type, uint16_t flags);
    void send_heartbeat(Peer& p);
    void send_via_relay(const uint8_t* payload, uint16_t len, uint8_t msg_type);
    void apply_delta(Peer& p, const uint8_t* payload, uint16_t len, NetPacket& out);
    void apply_full_state(Peer& p, const uint8_t* payload, uint16_t len, NetPacket& out);
    bool consume_tokens(Peer& p, size_t bytes);
    void refill_tokens(Peer& p);
    uint16_t build_entity_state(const float* positions, uint32_t count, Peer& p, std::vector<uint8_t>& out);
    NetPacket make_interp_packet() const;
    void process_ack(Peer& p, uint16_t ack_seq, uint16_t ack_bits);
    static bool addr_equal(const sockaddr_in& a, const sockaddr_in& b);
};

} // namespace tehi
