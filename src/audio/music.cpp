#include "music.h"
#include <cstdio>
#include <filesystem>

namespace tehi {

static void scan_playlist_dir(MusicManager& mgr) {
    namespace fs = std::filesystem;
    const fs::path dir = "assets/audio/music";
    for (const auto& entry : fs::directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;
        auto ext = entry.path().extension();
        if (ext == ".ogg" || ext == ".mp3" || ext == ".wav" || ext == ".flac") {
            Track t;
            t.id = (uint32_t)(mgr.playlist().size() + 1);
            std::snprintf(t.path, sizeof(t.path), "%s", entry.path().string().c_str());
            std::snprintf(t.title, sizeof(t.title), "%s", entry.path().stem().string().c_str());
            mgr.add_track(t);
        }
    }
}

bool MusicManager::initialize() {
    scan_playlist_dir(*this);
    if (m_playlist.empty()) {
        m_playlist.push_back({1, "assets/audio/music/track_01.ogg", "Main Theme", 1.0f, true});
    }
    std::printf("[Music] Playlist loaded: %zu tracks\n", m_playlist.size());
    return true;
}

void MusicManager::shutdown() {
    stop();
    m_playlist.clear();
    std::printf("[Music] Shutdown\n");
}

bool MusicManager::play_track(uint32_t track_id) {
    for (size_t i = 0; i < m_playlist.size(); ++i) {
        if (m_playlist[i].id == track_id) {
            m_current_index = (uint32_t)i;
            m_current = &m_playlist[i];
            m_playing = true;
            std::printf("[Music] Playing: %s\n", m_current->title);
            return true;
        }
    }
    return false;
}

bool MusicManager::play_next() {
    if (m_playlist.empty()) return false;
    m_current_index = (m_current_index + 1) % m_playlist.size();
    m_current = &m_playlist[m_current_index];
    m_playing = true;
    std::printf("[Music] Next: %s\n", m_current->title);
    return true;
}

void MusicManager::stop() {
    if (m_playing) {
        std::printf("[Music] Stopped: %s\n", m_current ? m_current->title : "(none)");
        m_playing = false;
    }
}

void MusicManager::set_volume(float volume) {
    m_volume = volume;
    std::printf("[Music] Volume set to %.2f\n", volume);
}

void MusicManager::update(float dt) {
    (void)dt;
    // stub: playback position, crossfade, auto-next
}

} // namespace tehi
