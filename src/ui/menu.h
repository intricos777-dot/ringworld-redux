#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace tehi {

struct MenuOption {
    uint32_t id;
    std::string label;
    bool active = false;
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
    uint32_t get_selected_id() const { return m_selected_index; }

    bool confirm_selection();

    void set_active(bool active) { m_active = active; }
    bool is_active() const { return m_active; }

private:
    bool m_active = true;
    uint32_t m_selected_index = 0;
    std::vector<MenuOption> m_options;
};

} // namespace tehi
