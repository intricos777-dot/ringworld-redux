#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace tehi {

struct TransitionClip {
    uint32_t id;
    char path[256];
    char name[64];
    float duration = 1.0f;
};

class TransitionSystem {
public:
    TransitionSystem() = default;
    ~TransitionSystem() = default;
    bool initialize();
    void trigger(uint32_t clip_id);
    void update(float dt);
    bool is_playing() const { return m_playing; }
    float get_progress() const { return m_progress; }
private:
    std::vector<TransitionClip> m_clips;
    bool m_playing = false;
    float m_progress = 0.0f;
    float m_duration = 1.0f;
};

} // namespace tehi
