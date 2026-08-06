#include "audio_script.h"
#include "audio_system.h"
#include <cstdio>

namespace tehi {

bool AudioScript::load(const std::string& path) {
    (void)path;
    // Stub parser: replace with JSON/TOML script later
    std::printf("[AudioScript] Loaded stub script\n");
    return true;
}

void AudioScript::execute(const std::string& trigger) {
    if (trigger == "awakening_spawn") {
        get_music_manager().play_track(1);
        get_transition_system().trigger(1);
    } else if (trigger == "checkpoint") {
        get_transition_system().trigger(4);
    } else if (trigger == "oracle_contact") {
        get_voice_manager().get_bank(VoiceType::Lia);
        get_transition_system().trigger(1);
    }
    std::printf("[AudioScript] Executed trigger=%s\n", trigger.c_str());
}

void AudioScript::update(float dt) {
    (void)dt;
    // Stub: handle timed voice/music queuing
}

} // namespace tehi
