#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace tehi {

struct Track {
    uint32_t id;
    char path[256];
    char title[64];
    float volume = 1.0f;
    bool loop = true;
};

class MusicManager {
public:
    MusicManager() = default;
    ~MusicManager() = default;
    bool initialize();
    void shutdown();
    bool play_track(uint32_t track_id);
    bool play_next();
    void stop();
    void set_volume(float volume);
    void update(float dt);
    const Track* current() const { return m_current; }
    const std::vector<Track>& playlist() const { return m_playlist; }
    void add_track(const Track& t) { m_playlist.push_back(t); }
private:
    std::vector<Track> m_playlist;
    uint32_t m_current_index = 0;
    Track* m_current = nullptr;
    float m_volume = 1.0f;
    bool m_playing = false;
};

} // namespace tehi
