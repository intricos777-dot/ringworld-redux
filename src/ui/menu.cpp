#include "menu.h"
#include <cstdio>

namespace tehi {

static const char* MENU_LABELS[] = {
    "Start Campaign",
    "Start Sandbox",
    "Host Game",
    "Join Game",
    "Quit"
};

bool MainMenu::initialize() {
    m_active = true;
    m_selected_action = MenuAction::StartCampaign;
    std::printf("[Menu] Main menu initialized\n");
    return true;
}

void MainMenu::shutdown() {
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
    for (size_t i = 0; i < sizeof(MENU_LABELS) / sizeof(MENU_LABELS[0]); ++i) {
        MenuAction act = (MenuAction)(i + 1);
        const char* sel = (act == m_selected_action) ? ">" : " ";
        std::printf("[Menu] %s %s\n", sel, MENU_LABELS[i]);
    }
    std::printf("[Menu] ==============================\n");
}

void MainMenu::move_selection(int direction) {
    int count = (int)(sizeof(MENU_LABELS) / sizeof(MENU_LABELS[0]));
    int cur = (int)m_selected_action - 1;
    cur = (cur + direction + count) % count;
    m_selected_action = (MenuAction)(cur + 1);
}

} // namespace tehi
