#include "game_mode.h"
#include <cstdio>

namespace tehi {

static GameModeSystem g_game_mode;

void GameModeSystem::set_mode(GameMode mode) {
    m_mode = mode;
    if (mode == GameMode::Legend) {
        m_player_damage_scale = 1.0f;
        m_enemy_damage_scale = 1.0f;
        m_ai_reaction_time = 0.05f;
        m_friendly_fire = true;
        m_unlimited_abilities = false;
        std::printf("[Mode] Legend mode: human + alien forces unrestricted within ability/weapon limits\n");
    } else {
        m_player_damage_scale = 1.2f;
        m_enemy_damage_scale = 0.8f;
        m_ai_reaction_time = 0.5f;
        m_friendly_fire = false;
        m_unlimited_abilities = false;
        std::printf("[Mode] Casual mode: forgiving damage, slower AI\n");
    }
}

GameModeSystem& get_game_mode_system() {
    return g_game_mode;
}

} // namespace tehi
