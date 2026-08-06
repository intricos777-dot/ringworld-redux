#include "map.h"
#include <cstdio>

namespace tehi {

bool Map::load(const std::string& path) {
    std::printf("[World] Map loaded from %s (stub)\n", path.c_str());
    m_spawns.push_back({{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0});
    return true;
}

} // namespace tehi
