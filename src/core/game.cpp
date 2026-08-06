#include "game.h"
#include "world.h"
#include <cstdio>
#include <chrono>
#include <thread>

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
    using clock = std::chrono::steady_clock;
    auto next_tick = clock::now();
    const auto tick_duration = std::chrono::milliseconds(16);

    std::printf("[Game] Running...\n");
    while (m_running) {
        std::this_thread::sleep_until(next_tick);
        next_tick += tick_duration;

        float dt = 1.0f / 60.0f;
        if (m_world) m_world->update(dt);
        update_campaign(dt);
        render_frame();
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

void Game::update_campaign(float dt) {
    // TODO: integrate mission/mission_update once main wires it in
}

void Game::render_frame() const {
    // Stub render frame for now
}

} // namespace tehi
