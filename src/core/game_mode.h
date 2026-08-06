#pragma once
#include <cstdint>
#include <string>

namespace tehi {

enum class GameMode : uint32_t {
    Casual,
    Legend
};

class GameModeSystem {
public:
    GameModeSystem() = default;
    ~GameModeSystem() = default;

    void set_mode(GameMode mode);
    GameMode get_mode() const { return m_mode; }

    bool is_legend() const { return m_mode == GameMode::Legend; }
    bool is_casual() const { return m_mode == GameMode::Casual; }

    float get_player_damage_scale() const { return m_player_damage_scale; }
    float get_enemy_damage_scale() const { return m_enemy_damage_scale; }
    float get_ai_reaction_time() const { return m_ai_reaction_time; }
    bool get_friendly_fire_enabled() const { return m_friendly_fire; }
    bool get_unlimited_abilities() const { return m_unlimited_abilities; }

private:
    GameMode m_mode = GameMode::Casual;
    float m_player_damage_scale = 1.0f;
    float m_enemy_damage_scale = 1.0f;
    float m_ai_reaction_time = 0.5f;
    bool m_friendly_fire = false;
    bool m_unlimited_abilities = false;
};

GameModeSystem& get_game_mode_system();

} // namespace tehi
