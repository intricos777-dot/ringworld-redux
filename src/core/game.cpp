#include "game.h"
#include "world.h"
#include "player/controller.h"
#include "ui/hud.h"
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
    m_controller = std::make_unique<PlayerController>();
    m_controller->initialize();
    m_hud = std::make_unique<HUD>();
    m_hud->initialize();
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
        update_input(dt);
        update_campaign(dt);
        render_frame();
    }
}

void Game::shutdown() {
    if (!m_initialized) return;
    std::printf("[Game] Shutting down\n");
    m_running = false;
    m_initialized = false;
    m_hud.reset();
    m_controller.reset();
    m_world.reset();
}

Game& Game::instance() {
    static Game instance;
    return instance;
}

void Game::update_input(float dt) {
    (void)dt;
    if (!m_controller) return;
    if (m_controller->wants_fire()) {
        std::printf("[Input] Fire pressed\n");
    }
    if (m_controller->wants_reload()) {
        std::printf("[Input] Reload pressed\n");
    }
    if (m_controller->wants_next_weapon()) {
        std::printf("[Input] Next weapon\n");
    }
    if (m_controller->wants_prev_weapon()) {
        std::printf("[Input] Prev weapon\n");
    }
}

void Game::update_campaign(float dt) {
    (void)dt;
    // TODO: integrate mission/mission_update once main wires it in
}

void Game::render_frame() const {
    draw_hud();
}

void Game::draw_hud() const {
    if (!m_hud) return;
    m_hud->draw_health(85.0f);
    m_hud->draw_shield(50.0f);
    m_hud->draw_ammo(24, 96);
    m_hud->render();
}

} // namespace tehi
