#include "transition.h"
#include <cstdio>

namespace tehi {

bool TransitionSystem::initialize() {
    m_clips.push_back({1, "assets/audio/transitions/whoosh_01.ogg", "Whoosh Light", 0.4f});
    m_clips.push_back({2, "assets/audio/transitions/whoosh_02.ogg", "Whoosh Heavy", 0.7f});
    m_clips.push_back({3, "assets/audio/transitions/impact_01.ogg", "Impact", 0.3f});
    m_clips.push_back({4, "assets/audio/transitions/alert_01.ogg", "Alert", 0.5f});
    std::printf("[Transition] Loaded %zu clips\n", m_clips.size());
    return true;
}

void TransitionSystem::trigger(uint32_t clip_id) {
    for (const auto& c : m_clips) {
        if (c.id == clip_id) {
            m_playing = true;
            m_progress = 0.0f;
            m_duration = c.duration;
            std::printf("[Transition] Triggered: %s (%.2fs)\n", c.name, c.duration);
            return;
        }
    }
    std::printf("[Transition] Clip %u not found\n", clip_id);
}

void TransitionSystem::update(float dt) {
    if (!m_playing) return;
    m_progress += dt;
    if (m_progress >= m_duration) {
        m_playing = false;
        m_progress = 0.0f;
        std::printf("[Transition] Complete\n");
    }
}

} // namespace tehi
