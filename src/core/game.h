#pragma once
#include <vector>
#include <memory>
#include <cstdint>

namespace tehi {

class PlayerController;
class HUD;
class World;

class Game {
public:
    Game();
    ~Game();

    bool initialize();
    void run();
    void shutdown();

    static Game& instance();
    World* get_world() const { return m_world.get(); }

private:
    bool m_running = false;
    bool m_initialized = false;
    std::unique_ptr<World> m_world;
    std::unique_ptr<PlayerController> m_controller;
    std::unique_ptr<HUD> m_hud;

    uint32_t m_equipped_main = 3;      // M4A1_CARBINE
    uint32_t m_equipped_secondary = 1;  // M17_9MM
    uint32_t m_equipped_space = 12;     // PLASMA_RIFLE

    void update_input(float dt);
    void update_campaign(float dt);
    void render_frame() const;
    void draw_hud() const;
};

} // namespace tehi
