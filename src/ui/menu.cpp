#include "menu.h"
#include <cstdio>

namespace tehi {

bool MainMenu::initialize() {
    m_options.clear();
    m_options.push_back({1, "Casual", true});
    m_options.push_back({2, "Legend", false});
    m_selected_index = 0;
    m_active = true;
    std::printf("[Menu] Main menu initialized\n");
    return true;
}

void MainMenu::shutdown() {
    m_options.clear();
    m_active = false;
}

void MainMenu::update(float dt) {
    (void)dt;
    if (!m_active) return;
    // Input handling stub
}

void MainMenu::render() const {
    if (!m_active) return;
    std::printf("\n[Menu] ==============================\n");
    for (size_t i = 0; i < m_options.size(); ++i) {
        const char* sel = (i == m_selected_index) ? ">" : " ";
        std::printf("[Menu] %s %s\n", sel, m_options[i].label.c_str());
    }
    std::printf("[Menu] ==============================\n");
}

void MainMenu::move_selection(int direction) {
    if (m_options.empty()) return;
    m_selected_index = (uint32_t)(((int)m_selected_index + direction + (int)m_options.size()) % (int)m_options.size());
}

bool MainMenu::confirm_selection() {
    if (m_options.empty()) return false;
    std::printf("[Menu] Selected: %s\n", m_options[m_selected_index].label.c_str());
    if (m_options[m_selected_index].id == 1) {
        std::printf("[Menu] Starting new game in Casual mode\n");
    }
    return true;
}

} // namespace tehi
