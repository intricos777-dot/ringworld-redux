#include "mission.h"
#include "audio/audio_system.h"
#include <nlohmann/json.hpp>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <cmath>

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

        m_id = root.value("mission_id", "");
        m_name = root.value("name", "");
        m_description = root.value("description", "");
        m_map = root.value("map", "");
        m_finale = root.value("finale", false);

        if (root.contains("objectives")) {
            for (const auto& obj : root["objectives"]) {
                MissionObjective o;
                o.id = obj.value("id", 0u);
                o.description = obj.value("description", "");
                o.completed = obj.value("completed", false);
                m_objectives.push_back(o);
            }
        }

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

        if (root.contains("dialogue")) {
            for (const auto& d : root["dialogue"]) {
                DialogueLine line;
                line.trigger = d.value("trigger", "");
                line.speaker = d.value("speaker", "");
                line.line = d.value("line", "");
                m_dialogue.push_back(line);
            }
        }

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

        if (root.contains("hidden_spawns")) {
            for (const auto& hs : root["hidden_spawns"]) {
                HiddenSpawn spawn;
                spawn.id = hs.value("id", 0u);
                spawn.position[0] = hs.value("x", 0.0f);
                spawn.position[1] = hs.value("y", 0.0f);
                spawn.position[2] = hs.value("z", 0.0f);
                spawn.enemy_type = hs.value("enemy_type", "");
                spawn.count = hs.value("count", 0u);
                m_hidden_spawns.push_back(spawn);
            }
        }
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

} // namespace tehi
