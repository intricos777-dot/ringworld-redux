#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace tehi {

enum class AudioScriptAction : uint32_t {
    PlayMusic,
    StopMusic,
    PlayVoice,
    PlayTransition,
    SetMasterVolume
};

struct AudioScriptLine {
    AudioScriptAction action;
    uint32_t target_id;
    float value;
    const char* text;
};

class AudioScript {
public:
    AudioScript() = default;
    ~AudioScript() = default;
    bool load(const std::string& path);
    void execute(const std::string& trigger);
    void update(float dt);
private:
    std::vector<AudioScriptLine> m_lines;
};

} // namespace tehi
