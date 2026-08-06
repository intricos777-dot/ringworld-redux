#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace tehi {

enum class VoiceType : uint32_t { Protagonist, Lia, Marine, Navy, AirForce, Grey, Reptilian, Cthulhu };

struct VoiceBank {
    VoiceType type;
    char name[32];
    char tts_voice[64];
    char sample_dir[256];
};

class VoiceManager {
public:
    VoiceManager() = default;
    ~VoiceManager() = default;
    bool initialize();
    const VoiceBank* get_bank(VoiceType type) const;
    const std::vector<VoiceBank>& banks() const { return m_banks; }
private:
    std::vector<VoiceBank> m_banks;
};

} // namespace tehi
