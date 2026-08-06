#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace tehi {

enum class Branch : uint32_t { Marines, Navy, AirForce, Special };

struct CharacterProfile {
    uint32_t id;
    char callsign[16];
    char full_name[48];
    Branch branch;
    char rank[24];
    char role[32];
    char backstory[256];
    bool is_protagonist = false;
};

class Roster {
public:
    static Roster& instance();
    bool load();
    const std::vector<CharacterProfile>& all() const;
    const CharacterProfile* find_by_callsign(const char* callsign) const;
    const CharacterProfile* find_by_id(uint32_t id) const;
    const CharacterProfile* get_protagonist() const;
private:
    std::vector<CharacterProfile> m_profiles;
    uint32_t m_protagonist_id = 0;
};

} // namespace tehi
