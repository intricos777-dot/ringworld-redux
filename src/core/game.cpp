#include "game.h"
#include "world.h"
#include <cstdio>

namespace tehi {

Game::Game() = default;
Game::~Game() { shutdown(); }

bool Game::initialize() {
    if (m_initialized) return true;
    std::printf("[Game] Initializing TE Halo-Inspired\n");
    m_world = std::make_unique<World>();
    m_world->initialize();
    m_running = true;
    m_initialized = true;
    return true;
}

void Game::run() {
    while (m_running) {
        // game loop stub
    }
}

void Game::shutdown() {
    if (!m_initialized) return;
    std::printf("[Game] Shutting down\n");
    m_running = false;
    m_initialized = false;
    m_world.reset();
}

Game& Game::instance() {
    static Game instance;
    return instance;
}

} // namespace tehi
