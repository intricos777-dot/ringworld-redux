#include "alien_panic.h"
#include <cstdio>
#include <algorithm>

namespace tehi {

AlienSquadManager::AlienSquadManager() = default;
AlienSquadManager::~AlienSquadManager() = default;

bool AlienSquadManager::initialize() {
    std::printf("[AlienPanic] System initialized\n");
    return true;
}

void AlienSquadManager::add_alien(uint32_t entity_id, AIAlienRank rank, uint32_t squad_id) {
    AlienAgent agent;
    agent.entity_id = entity_id;
    agent.rank = rank;
    agent.squad_id = squad_id;
    agent.panic_state = AIPanicState::Calm;
    agent.panic_meter = 0.0f;
    agent.has_elite_support = (rank == AIAlienRank::Grunt);
    m_aliens.push_back(agent);
}

void AlienSquadManager::remove_alien(uint32_t entity_id) {
    m_aliens.erase(
        std::remove_if(m_aliens.begin(), m_aliens.end(),
            [entity_id](const AlienAgent& a) { return a.entity_id == entity_id; }),
        m_aliens.end()
    );
}

void AlienSquadManager::notify_elite_killed(uint32_t squad_id) {
    for (auto& alien : m_aliens) {
        if (alien.squad_id == squad_id && alien.rank == AIAlienRank::Grunt) {
            alien.has_elite_support = false;
        }
    }
}

AIPanicState AlienSquadManager::get_panic_state(uint32_t entity_id) const {
    for (const auto& alien : m_aliens) {
        if (alien.entity_id == entity_id) {
            return alien.panic_state;
        }
    }
    return AIPanicState::Calm;
}

const char* AlienSquadManager::get_panic_line(AIPanicState state) const {
    switch (state) {
        case AIPanicState::Calm: return "...";
        case AIPanicState::Alert: return "Contact!";
        case AIPanicState::Panicked: return " formations breaking!";
        case AIPanicState::Ballistic: return "ITS A DEMON!!!!";
        default: return "";
    }
}

void AlienSquadManager::update(float dt) {
    m_panic_check_timer += dt;
    if (m_panic_check_timer < m_panic_check_interval) return;
    m_panic_check_timer = 0.0f;

    for (auto& alien : m_aliens) {
        if (alien.rank == AIAlienRank::Grunt && !alien.has_elite_support) {
            alien.panic_meter += 15.0f; // per-check increment (~7.5/sec at 2s interval)
            if (alien.panic_meter >= alien.panic_threshold) {
                alien.panic_state = AIPanicState::Ballistic;
                std::printf("[AlienPanic] %u: %s\n", alien.entity_id, get_panic_line(AIPanicState::Ballistic));
            } else if (alien.panic_meter >= alien.panic_threshold * 0.6f) {
                alien.panic_state = AIPanicState::Panicked;
            } else if (alien.panic_meter >= alien.panic_threshold * 0.3f) {
                alien.panic_state = AIPanicState::Alert;
            }
        }
    }
}

} // namespace tehi
