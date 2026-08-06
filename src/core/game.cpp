#include "game.h"
#include "world.h"
#include "player/controller.h"
#include "ui/hud.h"
#include "ui/menu.h"
#include "ai/squad.h"
#include "weapons/weapon_registry.h"
#include "weapons/inventory.h"
#include "core/game_mode.h"
#include "core/easter_eggs.h"
#include "core/achievements.h"
#include "renderer/render_bridge.h"
#include "audio/audio_script.h"
#include <cstdio>
#include <chrono>
#include <thread>

namespace tehi {

Game::Game() = default;
Game::~Game() { shutdown(); }

bool Game::initialize() {
    if (m_initialized) return true;
    std::printf("[Game] Initializing Ringworld Redux\n");
    m_world = std::make_unique<World>();
    m_world->initialize();
    m_controller = std::make_unique<PlayerController>();
    m_controller->initialize();
    m_hud = std::make_unique<HUD>();
    m_hud->initialize();
    m_menu = std::make_unique<MainMenu>();
    m_menu->initialize();
    m_renderer = std::make_unique<RenderBridge>();
    m_renderer->initialize(nullptr);
    m_audio_script = std::make_unique<AudioScript>();
    m_audio_script->load("maps/signal_lost.json");
    AchievementSystem::instance().initialize();
    spawn_initial_entities();
    m_controller->set_controller_type(ControllerType::XboxOne);
    std::printf("[Game] Main menu — controller: Xbox One | Fire / Enter to start\n");
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
        get_easter_egg_system().update(dt);
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
        uint32_t id = m_active_slot == 0 ? m_equipped_main : m_active_slot == 1 ? m_equipped_secondary : m_equipped_space;
        const auto* spec = get_weapon_spec((RealWeaponID)id);
        if (spec) {
            std::printf("[Weapon] Fired %s dmg=%.1f\n", spec->display_name, spec->damage);
            apply_weapon_damage(0.0f, 0.0f, 0.0f, spec->effective_range, spec->damage);
        }
    }
    if (m_controller->wants_reload()) {
        std::printf("[Input] Reload pressed\n");
    }
    if (m_controller->wants_next_weapon()) {
        cycle_weapon(1);
    }
    if (m_controller->wants_prev_weapon()) {
        cycle_weapon(-1);
    }
}

void Game::update_campaign(float dt) {
    (void)dt;
    // TODO: integrate mission/mission_update once main wires it in
}

void Game::render_frame() const {
    if (m_renderer) {
        m_renderer->begin_frame();
        draw_hud();
        m_renderer->end_frame();
        m_renderer->present();
    } else {
        draw_hud();
    }
}

void Game::draw_hud() const {
    if (!m_hud) return;
    m_hud->draw_health(85.0f);
    m_hud->draw_shield(50.0f);
    m_hud->draw_ammo(24, 96);
    m_hud->render();
}

void Game::apply_loadout() {
    std::printf("[Game] Applying loadout: main=%u secondary=%u space=%u\n",
        m_equipped_main, m_equipped_secondary, m_equipped_space);
}

void Game::spawn_initial_entities() {
    if (!m_world) return;
    // squad 1 = player
    // squad 2 = aliens
    m_world->spawn_entity(1, 0.0f, 0.0f, 0.0f);
    m_world->spawn_entity(2, 2.0f, 0.0f, 5.0f);
    m_world->spawn_entity(3, -2.0f, 0.0f, 5.0f);
    m_world->spawn_entity(4, 10.0f, 0.0f, 0.0f);

    // Register aliens for panic tracking
    m_world->register_alien(2, 2, 1, 2); // grunt
    m_world->register_alien(3, 2, 1, 2); // grunt
    m_world->register_alien(4, 2, 2, 2); // elite
    std::printf("[Game] Initial entities spawned\n");
}

void Game::apply_weapon_damage(float x, float y, float z, float radius, float damage) {
    if (!m_world) return;
    auto& entities = m_world->get_entities();
    bool hit = false;
    for (auto& e : entities) {
        if (!e.active || e.type == 1) continue;
        float dx = e.position[0] - x;
        float dy = e.position[1] - y;
        float dz = e.position[2] - z;
        float dist = dx*dx + dy*dy + dz*dz;
        if (dist <= radius * radius) {
            std::printf("[Weapon] Hit entity %u at dist %.2f dmg=%.1f\n", e.id, dist, damage);
            hit = true;
        }
    }
    if (!hit) std::printf("[Weapon] Missed\n");
}

void Game::cycle_weapon(int direction) {
    uint32_t slots[3] = { m_equipped_main, m_equipped_secondary, m_equipped_space };
    m_active_slot = (uint32_t)(((int)m_active_slot + direction + 3) % 3);
    const char* slot_name = m_active_slot == 0 ? "Main" : m_active_slot == 1 ? "Secondary" : "Space";
    const uint32_t id = slots[m_active_slot];
    const auto* spec = get_weapon_spec((RealWeaponID)id);
    std::printf("[Game] Switched to %s: %s\n", slot_name, spec ? spec->display_name : "(unknown)");
}

} // namespace tehi
