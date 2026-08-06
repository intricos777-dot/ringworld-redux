#include "mission.h"
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
}

bool Mission::load_checkpoint() {
    std::printf("[Campaign] Checkpoint loaded\n");
    return true;
}

} // namespace tehi
