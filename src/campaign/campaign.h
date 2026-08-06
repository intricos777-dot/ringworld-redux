#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace tehi {

struct CampaignMission {
    std::string id;
    std::string name;
    std::string description;
    std::string map;
};

class Campaign {
public:
    Campaign() = default;
    ~Campaign() = default;

    bool initialize();
    void shutdown() {}

    const std::vector<CampaignMission>& missions() const { return m_missions; }
    const CampaignMission* current() const { return m_missions.empty() ? nullptr : &m_missions[m_index]; }
    bool advance() { if (m_index + 1 < m_missions.size()) { ++m_index; return true; } return false; }
    void reset() { m_index = 0; }
    size_t index() const { return m_index; }
    size_t count() const { return m_missions.size(); }

private:
    std::vector<CampaignMission> m_missions;
    size_t m_index = 0;
};

} // namespace tehi
