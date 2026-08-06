#pragma once
#include <cstdint>
#include "music.h"
#include "transition.h"
#include "voice.h"

namespace tehi {

class AudioEngine {
public:
    AudioEngine() = default;
    ~AudioEngine() = default;
    bool initialize();
    void shutdown();
    void update(float dt);
    void set_master_volume(float volume);
};

MusicManager& get_music_manager();
TransitionSystem& get_transition_system();
VoiceManager& get_voice_manager();

} // namespace tehi
