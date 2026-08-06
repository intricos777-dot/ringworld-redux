#include "game_state.h"
#include "world.h"
#include <cstdio>

namespace tehi {

static GameState g_game_state;

bool GameState::initialize() {
    m_world = new World();
    if (!m_world->initialize()) return false;
    std::printf("[State] GameState initialized\n");
    return true;
}

void GameState::shutdown() {
    if (m_world) {
        delete m_world;
        m_world = nullptr;
    }
    std::printf("[State] GameState shutdown\n");
}

void GameState::update(float dt) {
    if (m_main_menu || m_game_over || !m_world) return;
    m_world->update(dt);
}

void GameState::start_game() {
    m_main_menu = false;
    m_game_over = false;
    std::printf("[State] Game started\n");
}

GameState& get_game_state() {
    return g_game_state;
}

} // namespace tehi
