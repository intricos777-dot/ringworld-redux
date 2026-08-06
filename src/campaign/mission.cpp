#include "mission.h"
#include "audio/audio_system.h"
#include <cstdio>

namespace tehi {

bool Mission::initialize(const std::string& path) {
    std::printf("[Campaign] Mission loaded: %s\n", path.c_str());
    m_objectives.push_back({1, "Reach the extraction point", false});
    return true;
}

bool Mission::update(float dt) {
    (void)dt;
    for (auto& obj : m_objectives) {
        if (!obj.completed) {
            std::printf("[Campaign] Objective: %s\n", obj.description.c_str());
            break;
        }
    }
    return true;
}

void Mission::save_checkpoint() {
    std::printf("[Campaign] Checkpoint saved\n");
    auto& trans = get_transition_system();
    trans.trigger(4); // Alert clip on checkpoint
}

bool Mission::load_checkpoint() {
    std::printf("[Campaign] Checkpoint loaded\n");
    auto& trans = get_transition_system();
    trans.trigger(1); // Light whoosh on load
    return true;
}

} // namespace tehi
