#include "mission.h"
#include "audio/audio_system.h"
#include <cstdio>

namespace tehi {

bool Mission::initialize(const std::string& path) {
    std::printf("[Campaign] Mission loaded: %s\n", path.c_str());
    m_objectives.push_back({1, "Reach the extraction point", false});
    m_dialogue.push_back({"on_spawn", "Lia", "Systems online. I am with you. Let us move forward."});
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
    for (const auto& d : m_dialogue) {
        if (d.trigger == "on_spawn") {
            std::printf("[Campaign] %s: %s\n", d.speaker.c_str(), d.line.c_str());
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
