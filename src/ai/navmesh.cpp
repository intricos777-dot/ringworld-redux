#include "bot.h"
#include <cstdio>

namespace tehi {

bool NavMesh::load(const std::string& path) {
    std::printf("[AI] NavMesh loaded from %s (stub)\n", path.c_str());
    return true;
}
const NavPoint* NavMesh::find_nearest(float x, float y, float z) const {
    (void)x; (void)y; (void)z;
    if (m_points.empty()) return nullptr;
    return &m_points[0];
}

bool Bot::initialize(uint32_t entity_id) {
    m_entity_id = entity_id;
    std::printf("[AI] Bot initialized entity %u\n", entity_id);
    return true;
}
void Bot::update(float dt) {
    (void)dt;
    // stub behavior: patrol, chase, attack
}

} // namespace tehi
