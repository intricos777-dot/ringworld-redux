#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace tehi {

enum class MenuAction : uint32_t {
    None = 0,
    StartCampaign,
    StartSandbox,
    HostGame,
    JoinGame,
    Settings,
    Quit
};

class MainMenu {
public:
    MainMenu() = default;
    ~MainMenu() = default;

    bool initialize();
    void shutdown();

    void update(float dt);
    void render() const;

    void move_selection(int direction);
    MenuAction get_selected_action() const { return m_selected_action; }

    void set_active(bool active) { m_active = active; }
    bool is_active() const { return m_active; }

private:
    bool m_active = true;
    MenuAction m_selected_action = MenuAction::StartCampaign;
};

} // namespace tehi
