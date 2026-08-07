#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace tehi {

enum class AIAlienRank : uint32_t {
    Grunt = 0,
    Elite
};

enum class AIPanicState : uint32_t {
    Calm,
    Alert,
    Panicked,
    Ballistic
};

struct AlienAgent {
    uint32_t entity_id = 0;
    AIAlienRank rank = AIAlienRank::Grunt;
    AIPanicState panic_state = AIPanicState::Calm;
    float panic_meter = 0.0f;
    float panic_threshold = 70.0f;
    bool has_elite_support = true;
    uint32_t squad_id = 0;
};

class AlienSquadManager {
public:
    AlienSquadManager();
    ~AlienSquadManager();
    bool initialize();
    void add_alien(uint32_t entity_id, AIAlienRank rank, uint32_t squad_id);
    void remove_alien(uint32_t entity_id);
    void update(float dt);
    void notify_elite_killed(uint32_t squad_id);
    AIPanicState get_panic_state(uint32_t entity_id) const;
    const char* get_panic_line(AIPanicState state) const;
    const std::vector<AlienAgent>& get_aliens() const { return m_aliens; }
private:
    std::vector<AlienAgent> m_aliens;
    float m_panic_check_timer = 0.0f;
    float m_panic_check_interval = 2.0f;
};

} // namespace tehi
