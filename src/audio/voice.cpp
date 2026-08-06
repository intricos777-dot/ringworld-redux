#include "voice.h"
#include <cstdio>

namespace tehi {

static const VoiceBank BANKS[] = {
    {VoiceType::Protagonist, "Living_Sin", "en-US-GuyNeural", "assets/audio/voices/protagonist/"},
    {VoiceType::Lia, "Lia", "en-US-JennyNeural", "assets/audio/voices/lia/"},
    {VoiceType::Marine, "Marine", "en-US-GuyNeural", "assets/audio/voices/marine/"},
    {VoiceType::Navy, "Navy", "en-US-GuyNeural", "assets/audio/voices/navy/"},
    {VoiceType::AirForce, "AirForce", "en-US-GuyNeural", "assets/audio/voices/airforce/"},
    {VoiceType::Grey, "Grey", "en-US-GuyNeural", "assets/audio/voices/grey/"},
    {VoiceType::Reptilian, "Reptilian", "en-US-GuyNeural", "assets/audio/voices/reptilian/"},
    {VoiceType::Cthulhu, "Cthulhu", "en-US-GuyNeural", "assets/audio/voices/cthulhu/"},
};

bool VoiceManager::initialize() {
    m_banks.assign(std::begin(BANKS), std::end(BANKS));
    std::printf("[Voice] Loaded %zu voice banks\n", m_banks.size());
    return true;
}

const VoiceBank* VoiceManager::get_bank(VoiceType type) const {
    for (const auto& b : m_banks) {
        if (b.type == type) return &b;
    }
    return nullptr;
}

} // namespace tehi
