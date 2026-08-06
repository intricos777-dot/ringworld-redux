#include "campaign.h"
#include "mission.h"
#include <cstdio>

namespace tehi {

static const char* CAMPAIGN_MISSION_IDS[] = {
    "/home/sin/Projects/ringworld-redux/maps/primer.json",
    "/home/sin/Projects/ringworld-redux/maps/signal_lost.json",
    "/home/sin/Projects/ringworld-redux/maps/breach_zero.json",
    "/home/sin/Projects/ringworld-redux/maps/spire_echo.json",
    "/home/sin/Projects/ringworld-redux/maps/deep_field.json",
    "/home/sin/Projects/ringworld-redux/maps/arc_collapse.json"
};

bool Campaign::initialize() {
    m_missions.clear();
    m_index = 0;
    for (const char* path : CAMPAIGN_MISSION_IDS) {
        tehi::Mission m;
        if (!m.initialize(path)) continue;
        CampaignMission cm;
        cm.id = m.id();
        cm.name = m.name();
        cm.description = m.description();
        cm.map = path;
        m_missions.push_back(cm);
    }
    std::printf("[Campaign] Loaded %u missions\n", (unsigned)m_missions.size());
    for (size_t i = 0; i < m_missions.size(); ++i) {
        std::printf("[Campaign] %u. %s — %s\n", (unsigned)i + 1, m_missions[i].id.c_str(), m_missions[i].name.c_str());
    }
    return !m_missions.empty();
}

} // namespace tehi
