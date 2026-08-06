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
#include "core/save_system.h"
#include "core/multiplayer/network.h"
#include "campaign/mission.h"
#include "renderer/sdl_gl_backend.h"
#include "audio/audio_script.h"
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <cmath>

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
    m_renderer = std::make_unique<SDLGLBackend>();
    if (!m_renderer->initialize(1280, 720, "Ringworld Redux")) {
        std::fprintf(stderr, "Failed to init renderer\n");
        return false;
    }
    m_audio_script = std::make_unique<AudioScript>();
    m_audio_script->load("maps/signal_lost.json");
    m_save = std::make_unique<SaveSystem>();
    m_save->initialize();
    m_save->load("savegame.sav");
    m_network = std::make_unique<NetworkSystem>();
    if (getenv("RR_AUTO_HOST") && m_network) {
        uint16_t port = (uint16_t)std::atoi(getenv("RR_AUTO_HOST"));
        if (port == 0) port = 7777;
        m_network->host(port);
    }
    m_mission = std::make_unique<Mission>();
    m_mission->initialize("../maps/arc_collapse.json");
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
    if (std::getenv("RR_TEST_FIRE")) {
        if (m_menu) m_menu->set_active(false);
        m_main_menu = false;
        if (m_world && !m_world->get_entities().empty()) {
            const auto& e = m_world->get_entities().front();
            std::printf("[Test] RR_TEST_FIRE -> apply_weapon_damage toward entity %u at (%.1f,%.1f,%.1f)\n", e.id, e.position[0], e.position[1], e.position[2]);
            apply_weapon_damage(e.position[0], e.position[1], e.position[2], 2.0f, 10.0f);
        }
    }
    int test_fire_frames = 0;
    while (m_running && !m_should_close) {
        std::this_thread::sleep_until(next_tick);
        next_tick += tick_duration;

        float dt = 1.0f / 60.0f;
        if (m_world) m_world->update(dt);
        if (m_network) {
            if (!m_world->get_entities().empty()) {
                static float net_buf[12 * 3];
                uint32_t n = 0;
                for (const auto& e : m_world->get_entities()) {
                    if (n >= 12) break;
                    net_buf[n * 3 + 0] = e.position[0];
                    net_buf[n * 3 + 1] = e.position[1];
                    net_buf[n * 3 + 2] = e.position[2];
                    ++n;
                }
                m_network->update(dt, net_buf, n);
            } else {
                m_network->update(dt);
            }
            if (!m_network->is_host()) {
                auto pkts = m_network->receive();
                for (const auto& pkt : pkts) {
                    if (pkt.type == 1) {
                        uint32_t count = *(const uint32_t*)pkt.data;
                        if (count > 12) count = 12;
                        auto& entities = m_world->get_entities();
                        for (uint32_t i = 0; i < count && i < entities.size(); ++i) {
                            entities[i].position[0] = pkt.data[1 + i * 3 + 0];
                            entities[i].position[1] = pkt.data[1 + i * 3 + 1];
                            entities[i].position[2] = pkt.data[1 + i * 3 + 2];
                        }
                    }
                }
            }
        }
        if (std::getenv("RR_TEST_FIRE") && test_fire_frames < 3) {
            ++test_fire_frames;
            m_controller->set_key_state(InputKey::Fire, true);
            m_controller->set_key_state(InputKey::AltFire, true);
            m_controller->set_key_state(InputKey::Reload, true);
            update_input(dt);
            m_controller->set_key_state(InputKey::Fire, false);
            m_controller->set_key_state(InputKey::AltFire, false);
            m_controller->set_key_state(InputKey::Reload, false);
        }
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
    if (m_menu && m_menu->is_active()) {
        if (m_controller->wants_fire() || m_controller->wants_next_weapon()) {
            switch (m_menu->get_selected_action()) {
                case MenuAction::Quit:
                    m_running = false;
                    return;
                case MenuAction::HostGame:
                    if (start_host()) {
                        m_menu->set_active(false);
                        m_main_menu = false;
                    }
                    return;
                case MenuAction::JoinGame:
                    start_join("127.0.0.1");
                    m_menu->set_active(false);
                    m_main_menu = false;
                    return;
                default:
                    break;
            }
        }
        if (m_controller->wants_prev_weapon()) {
            m_menu->move_selection(-1);
        } else if (m_controller->wants_next_weapon()) {
            m_menu->move_selection(1);
        }
        return;
    }
    float move_speed = m_controller->wants_sprint() ? 6.0f : 3.0f;
    float fwd = m_controller->wants_forward() ? 1.0f : m_controller->wants_back() ? -1.0f : 0.0f;
    float strafe = m_controller->wants_right() ? 1.0f : m_controller->wants_left() ? -1.0f : 0.0f;
    float cam_yaw = m_world ? m_world->get_camera().yaw : 0.0f;
    float cosy = std::cos(cam_yaw);
    float siny = std::sin(cam_yaw);
    m_player_velocity[0] = (cosy * fwd - siny * strafe) * move_speed;
    m_player_velocity[2] = (-siny * fwd - cosy * strafe) * move_speed;
    m_player_position[0] += m_player_velocity[0] * dt;
    m_player_position[1] += m_player_velocity[1] * dt;
    m_player_position[2] += m_player_velocity[2] * dt;
    if (m_world) {
        tehi::CameraState cam_state;
        cam_state.position[0] = m_player_position[0];
        cam_state.position[1] = m_player_position[1] + 1.6f;
        cam_state.position[2] = m_player_position[2];
        cam_state.yaw = cam_yaw;
        cam_state.pitch = m_world->get_camera().pitch;
        cam_state.fov = m_world->get_camera().fov;
        m_world->set_camera(cam_state);
    }
    if (m_controller->wants_fire()) {
        uint32_t id = m_active_slot == 0 ? m_equipped_main : m_active_slot == 1 ? m_equipped_secondary : m_equipped_space;
        const auto* spec = get_weapon_spec((RealWeaponID)id);
        if (spec) {
            std::printf("[Weapon] Fired %s dmg=%.1f\n", spec->display_name, spec->damage);
            apply_weapon_damage(m_player_position[0], m_player_position[1], m_player_position[2], spec->effective_range, spec->damage);
        }
    }
    if (m_controller->wants_alt_fire()) {
        uint32_t id = m_active_slot == 0 ? m_equipped_main : m_active_slot == 1 ? m_equipped_secondary : m_equipped_space;
        const auto* spec = get_weapon_spec((RealWeaponID)id);
        if (spec) {
            std::printf("[Weapon] Alt fire %s dmg=%.1f\n", spec->display_name, spec->damage);
            apply_weapon_damage(m_player_position[0], m_player_position[1], m_player_position[2], spec->effective_range * 0.5f, spec->damage * 2.0f);
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

bool Game::start_host(uint16_t port) {
    if (!m_network) return false;
    if (m_network->host(port)) {
        std::printf("[Game] Hosting multiplayer on port %u\n", (unsigned)port);
        return true;
    }
    std::fprintf(stderr, "[Game] Failed to host multiplayer\n");
    return false;
}

bool Game::start_join(const std::string& host, uint16_t port) {
    if (!m_network) return false;
    if (m_network->join(host, port)) {
        std::printf("[Game] Joining multiplayer at %s:%u\n", host.c_str(), (unsigned)port);
        return true;
    }
    std::fprintf(stderr, "[Game] Failed to join multiplayer\n");
    return false;
}

void Game::update_campaign(float dt) {
    (void)dt;
    if (!m_mission) return;
    const char* skip = getenv("RR_CAMPAIGN_SKIP");
    float player_pos[3] = {5.0f, -10.0f, 2.0f};
    if (skip && skip[0] == '1') {
        m_mission->update(dt, nullptr);
        return;
    }
    m_mission->update(dt, player_pos);
}

void Game::render_frame() const {
    if (m_renderer) {
        m_renderer->begin_frame();
        draw_hud();
        m_renderer->end_frame();
        m_renderer->present();
        if (m_renderer->poll_events(m_controller.get())) {
            m_should_close = true;
        }
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
        if (!e.active) continue;
        float dx = e.position[0] - x;
        float dy = e.position[1] - y;
        float dz = e.position[2] - z;
        float dist = dx*dx + dy*dy + dz*dz;
        if (dist <= radius * radius) {
            m_world->damage_entity(e.id, damage);
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
