#include "mission.h"
#include "audio/audio_system.h"
#include "core/achievements.h"
#include "core/game_mode.h"
#include "core/level_modifier.h"
#include "core/save_system.h"
#include "core/world.h"
#include "core/game.h"
#include <nlohmann/json.hpp>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstring>

namespace tehi {

using json = nlohmann::json;

namespace {
std::string slurp(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return {};
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}
}

bool Mission::initialize(const std::string& path) {
    std::printf("[Campaign] Mission loaded: %s\n", path.c_str());
    m_objectives.clear();
    m_dialogue.clear();
    m_enemies.clear();
    m_hidden_spawns.clear();
    m_waypoints.clear();
    m_finale = false;
    m_id.clear();
    m_name.clear();
    m_description.clear();
    m_map.clear();

    std::string text = slurp(path);
    if (text.empty()) {
        std::fprintf(stderr, "[Campaign] Failed to load mission: %s\n", path.c_str());
        return false;
    }

    try {
        json root = json::parse(text);
        std::printf("[Campaign] JSON parsed OK, keys=%zu\n", root.size());

        m_id = root.value("mission_id", "");
        std::printf("[Campaign] mission_id=%s\n", m_id.c_str());
        m_name = root.value("name", "");
        std::printf("[Campaign] name=%s\n", m_name.c_str());
        m_description = root.value("description", "");
        m_map = root.value("map", "");
        std::printf("[Campaign] map=%s\n", m_map.c_str());
        m_finale = root.value("finale", false);
        std::printf("[Campaign] finale=%d\n", (int)m_finale);

        if (root.contains("objectives")) {
            for (const auto& obj : root["objectives"]) {
                MissionObjective o;
                o.id = obj.value("id", 0u);
                o.description = obj.value("description", "");
                o.completed = obj.value("completed", false);
                m_objectives.push_back(o);
            }
        }
        std::printf("[Campaign] objectives=%zu\n", m_objectives.size());

        if (root.contains("waypoints")) {
            for (const auto& wp : root["waypoints"]) {
                Waypoint w;
                w.id = wp.value("id", 0u);
                w.position[0] = wp["position"].value("x", 0.0f);
                w.position[1] = wp["position"].value("y", 0.0f);
                w.position[2] = wp["position"].value("z", 0.0f);
                w.radius = wp.value("radius", 2.0f);
                m_waypoints.push_back(w);
            }
        }
        std::printf("[Campaign] waypoints=%zu\n", m_waypoints.size());

        if (root.contains("dialogue")) {
            for (const auto& d : root["dialogue"]) {
                DialogueLine line;
                line.trigger = d.value("trigger", "");
                line.speaker = d.value("speaker", "");
                line.line = d.value("line", "");
                m_dialogue.push_back(line);
            }
        }
        std::printf("[Campaign] dialogue=%zu\n", m_dialogue.size());

        if (root.contains("enemies")) {
            for (const auto& e : root["enemies"]) {
                EnemyEntry entry;
                entry.type = e.value("type", "");
                entry.count = e.value("count", 0u);
                entry.position[0] = e["position"].value("x", 0.0f);
                entry.position[1] = e["position"].value("y", 0.0f);
                entry.position[2] = e["position"].value("z", 0.0f);
                m_enemies.push_back(entry);
            }
        }
        std::printf("[Campaign] enemies=%zu\n", m_enemies.size());

        if (root.contains("hidden_spawns")) {
            for (const auto& hs : root["hidden_spawns"]) {
                HiddenSpawn spawn;
                spawn.id = hs.value("id", 0u);
                spawn.position[0] = hs.value("x", 0.0f);
                spawn.position[1] = hs.value("y", 0.0f);
                spawn.position[2] = hs.value("z", 0.0f);
                spawn.enemy_type = hs["enemy_type"].is_string() ? hs["enemy_type"].get<std::string>() : std::to_string(hs["enemy_type"].get<int>());
                spawn.count = hs.value("count", 0u);
                m_hidden_spawns.push_back(spawn);
            }
        }
        std::printf("[Campaign] hidden_spawns=%zu\n", m_hidden_spawns.size());

        if (root.contains("bosses")) {
            for (const auto& b : root["bosses"]) {
                BossEntry boss;
                boss.type = b.value("type", 0u);
                boss.count = b.value("count", 1u);
                boss.position[0] = b["position"].value("x", 0.0f);
                boss.position[1] = b["position"].value("y", 0.0f);
                boss.position[2] = b["position"].value("z", 0.0f);
                boss.health = b.value("health", 120.0f);
                m_bosses.push_back(boss);
            }
        }
        std::printf("[Campaign] bosses=%zu\n", m_bosses.size());
    } catch (const std::exception& ex) {
        std::fprintf(stderr, "[Campaign] JSON parse error in %s: %s\n", path.c_str(), ex.what());
        return false;
    } catch (...) {
        std::fprintf(stderr, "[Campaign] Unknown parse error in %s\n", path.c_str());
        return false;
    }

    std::printf("[Campaign] Loaded %u objectives, %u dialogue lines, %u enemy groups, %u hidden spawns\n",
        (unsigned)m_objectives.size(), (unsigned)m_dialogue.size(), (unsigned)m_enemies.size(), (unsigned)m_hidden_spawns.size());
    return true;
}

bool Mission::update(float dt, const float* player_position) {
    (void)dt;
    if (!m_started) {
        m_started = true;
        spawn_enemies();
        apply_scaling();
        spawn_bosses();
        if (get_game_mode_system().is_legend() && !m_hidden_spawns.empty()) {
            std::printf("[Campaign] Legend hidden spawns active: %zu\n", m_hidden_spawns.size());
            spawn_hidden_enemies();
        }
        trigger_dialogue("on_spawn");
    }

    for (auto& obj : m_objectives) {
        if (!obj.completed) {
            if (m_last_printed_objective_id != obj.id) {
                m_last_printed_objective_id = obj.id;
                std::printf("[Campaign] Objective: %s\n", obj.description.c_str());
            }
            break;
        }
    }
    if (!m_dialogue.empty() && m_dialogue[0].trigger == "on_spawn" && !m_on_spawn_printed) {
        m_on_spawn_printed = true;
        std::printf("[Campaign] %s: %s\n", m_dialogue[0].speaker.c_str(), m_dialogue[0].line.c_str());
    }
    if (!player_position) return true;
    if (!m_waypoints.empty()) {
        for (size_t i = 0; i < m_waypoints.size(); ++i) {
            const auto& wp = m_waypoints[i];
            float dx = player_position[0] - wp.position[0];
            float dy = player_position[1] - wp.position[1];
            float dz = player_position[2] - wp.position[2];
            float dist = std::sqrt(dx*dx + dy*dy + dz*dz);
            if (dist <= wp.radius) {
                if (i < m_objectives.size() && !m_objectives[i].completed) {
                    m_objectives[i].completed = true;
                    std::printf("[Campaign] Completed: %s\n", m_objectives[i].description.c_str());
                    trigger_dialogue(std::string("objective_") + std::to_string(m_objectives[i].id) + "_complete");
                    if (m_finale && i == m_objectives.size() - 1) {
                        complete_mission();
                    }
                }
                if (i + 1 < m_objectives.size() && !m_objectives[i + 1].completed) {
                    m_objectives[i + 1].completed = true;
                    std::printf("[Campaign] Waypoint %u reached: %s\n", (unsigned)wp.id, m_objectives[i + 1].description.c_str());
                }
                break;
            }
        }
    }
    return true;
}

void Mission::complete_objective(uint32_t id) {
    for (auto& obj : m_objectives) {
        if (obj.id == id) {
            obj.completed = true;
            std::printf("[Campaign] Objective completed: %s\n", obj.description.c_str());
            char trigger[64];
            std::snprintf(trigger, sizeof(trigger), "objective_%u_complete", (unsigned)id);
            trigger_dialogue(trigger);
            break;
        }
    }
}

void Mission::trigger_dialogue(const std::string& trigger) {
    for (const auto& d : m_dialogue) {
        if (d.trigger == trigger) {
            std::printf("[Campaign] %s: %s\n", d.speaker.c_str(), d.line.c_str());
            auto* game = Game::instance_ptr();
            if (game && game->get_audio_script()) {
                game->get_audio_script()->execute(trigger);
            }
            break;
        }
    }
}

void Mission::save_checkpoint() {
    std::printf("[Campaign] Checkpoint saved\n");
    auto& trans = get_transition_system();
    trans.trigger(4); // Alert clip on checkpoint
}

bool Mission::load_checkpoint() {
    std::printf("[Campaign] Checkpoint loaded\n");
    auto& trans = get_transition_system();
    trans.trigger(1); // Light whoosh on load
    return true;
}

void Mission::spawn(World* world) {
    if (!world) return;
    for (const auto& e : m_enemies) {
        for (uint32_t i = 0; i < e.count; ++i) {
            uint32_t t = enemy_type_to_uint(e.type);
            world->spawn_entity(t, e.position[0], e.position[1], e.position[2]);
        }
    }
    std::printf("[Campaign] Spawned %zu enemy groups\n", m_enemies.size());
}

void Mission::spawn_enemies() {
    auto* world = Game::instance().get_world();
    spawn(world);
}

void Mission::spawn_hidden_enemies() {
    auto* world = Game::instance().get_world();
    if (!world) return;
    for (const auto& hs : m_hidden_spawns) {
        uint32_t t = 0;
        try { t = std::stoi(hs.enemy_type); } catch (...) { t = 0; }
        for (uint32_t i = 0; i < hs.count; ++i) {
            world->spawn_entity(t, hs.position[0], hs.position[1], hs.position[2]);
        }
    }
    std::printf("[Campaign] Spawned %zu hidden enemy groups\n", m_hidden_spawns.size());
}

void Mission::spawn_bosses() {
    auto* world = Game::instance().get_world();
    if (!world) return;
    for (const auto& b : m_bosses) {
        for (uint32_t i = 0; i < b.count; ++i) {
            Entity* ent = world->spawn_entity(b.type, b.position[0], b.position[1], b.position[2]);
            if (ent && b.health > 0.0f) ent->health = b.health;
        }
    }
    if (!m_bosses.empty()) {
        std::printf("[Campaign] Spawned %zu boss groups\n", m_bosses.size());
    }
}

void Mission::apply_scaling() {
    auto& lm = get_level_modifier_system();
    uint32_t mission_level = (uint32_t)(m_index + 1u);
    if (mission_level <= 7) return;
    std::printf("[Campaign] Scaling active from level %u\n", mission_level);
    auto* world = Game::instance().get_world();
    if (!world) return;
    for (auto& e : world->get_entities()) {
        if (!e.active) continue;
        EnemyClass upgraded = lm.get_upgraded_enemy(mission_level, e.type);
        if (upgraded != (EnemyClass)e.type) {
            e.type = (uint32_t)upgraded;
            std::printf("[Campaign] Enemy %u upgraded to %u\n", e.id, e.type);
        }
    }
}
void Mission::complete_mission() {
    if (m_mission_completed) return;
    m_mission_completed = true;
    m_on_spawn_printed = true;
    std::printf("[Campaign] Mission complete: %s\n", m_name.c_str());
    trigger_dialogue("mission_complete");

    auto& achievements = AchievementSystem::instance();
    if (m_id == "tutorial" || m_id == "primer") achievements.unlock(Achievement::ACHIEVEMENT_CLEAR_AWAKENING);
    if (m_id == "signal_lost") achievements.unlock(Achievement::ACHIEVEMENT_CLEAR_FIRST_CONTACT);
    if (m_id == "breach_zero") achievements.unlock(Achievement::ACHIEVEMENT_CLEAR_TOWER_OF_BABEL);
    if (m_id == "spire_echo") achievements.unlock(Achievement::ACHIEVEMENT_CLEAR_FOUNDATION);
    if (m_finale) achievements.unlock(Achievement::ACHIEVEMENT_CLEAR_ALL_MISSIONS);
    if (get_game_mode_system().is_legend()) achievements.unlock(Achievement::ACHIEVEMENT_LEGEND_COMPLETE);

    auto* game = Game::instance_ptr();
    if (game && game->get_save_system()) {
        game->get_save_system()->save("savegame.sav");
    }
}

uint32_t Mission::enemy_type_to_uint(const std::string& type) const {
    if (type == "alien_grunt") return 1;
    if (type == "alien_elite") return 2;
    if (type == "seeker_bot") return 3;
    if (type == "alien_commander") return 4;
    if (type == "arc_core") return 5;
    return 0;
}

} // namespace tehi
