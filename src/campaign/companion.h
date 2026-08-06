#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace tehi {

enum class CompanionMood : uint32_t { Neutral, Alert, Concerned, Urgent };

struct CompanionLine {
    float trigger_health_pct;
    const char* line;
};

class StoryCompanion {
public:
    StoryCompanion() = default;
    ~StoryCompanion() = default;
    bool initialize();
    void update(float player_health, float shield, float dt);
    void say(const char* line);
    const char* get_name() const { return m_name; }
private:
    char m_name[32];
    char m_voice[32];
    float m_cooldown = 0.0f;
};

} // namespace tehi
