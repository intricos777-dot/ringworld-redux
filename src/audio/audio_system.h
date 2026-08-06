#pragma once
#include <cstdint>

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

} // namespace tehi
