#include "audio_system.h"
#include "music.h"
#include "transition.h"
#include "voice.h"
#include <cstdio>

namespace tehi {

static MusicManager g_music;
static TransitionSystem g_transitions;
static VoiceManager g_voice;

bool AudioEngine::initialize() {
    bool a = g_music.initialize();
    bool b = g_transitions.initialize();
    bool c = g_voice.initialize();
    std::printf("[Audio] Engine initialized music=%d transitions=%d voice=%d\n", a, b, c);
    return a && b && c;
}

void AudioEngine::shutdown() {
    g_music.shutdown();
    g_transitions.~TransitionSystem();
    std::printf("[Audio] Engine shutdown\n");
}

void AudioEngine::update(float dt) {
    g_music.update(dt);
    g_transitions.update(dt);
}

void AudioEngine::set_master_volume(float volume) {
    g_music.set_volume(volume);
}

MusicManager& get_music_manager() { return g_music; }
TransitionSystem& get_transition_system() { return g_transitions; }
VoiceManager& get_voice_manager() { return g_voice; }

} // namespace tehi
