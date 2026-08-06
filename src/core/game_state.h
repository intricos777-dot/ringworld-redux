#pragma once
#include <cstdint>

namespace tehi {

class World;

class GameState {
public:
    GameState() = default;
    ~GameState() = default;
    bool initialize();
    void shutdown();
    void update(float dt);
    World* get_world() const { return m_world; }

    void set_main_menu(bool active) { m_main_menu = active; }
    bool is_main_menu() const { return m_main_menu; }
    void start_game();
    bool is_game_over() const { return m_game_over; }
    void set_game_over(bool over) { m_game_over = over; }

private:
    bool m_main_menu = true;
    bool m_game_over = false;
    World* m_world = nullptr;
};

GameState& get_game_state();

} // namespace tehi
