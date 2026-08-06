#pragma once
#include <vector>
#include <memory>
#include <cstdint>

namespace tehi {

class PlayerController;
class HUD;
class World;
class MainMenu;
class AISquad;
class Inventory;
class SDLGLBackend;
class AudioScript;
class SaveSystem;
class NetworkSystem;
class AchievementSystem;

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
    mutable bool m_should_close = false;
    bool m_test_fire_ran = false;
    bool m_initialized = false;
    bool m_main_menu = true;
    std::unique_ptr<World> m_world;
    std::unique_ptr<PlayerController> m_controller;
    std::unique_ptr<HUD> m_hud;
    std::unique_ptr<MainMenu> m_menu;
    std::unique_ptr<AISquad> m_human_squad;
    std::unique_ptr<AISquad> m_alien_squad;
    std::unique_ptr<Inventory> m_human_inventory;
    std::unique_ptr<Inventory> m_alien_inventory;
    std::unique_ptr<SDLGLBackend> m_renderer;
    std::unique_ptr<AudioScript> m_audio_script;
    std::unique_ptr<SaveSystem> m_save;
    std::unique_ptr<NetworkSystem> m_network;

    uint32_t m_equipped_main = 3;      // M4A1_CARBINE
    uint32_t m_equipped_secondary = 1;  // M17_9MM
    uint32_t m_equipped_space = 12;     // PLASMA_RIFLE
    uint32_t m_active_slot = 0;         // 0=Main, 1=Secondary, 2=Space
    float m_player_position[3] = {0.0f, 0.0f, -5.0f};
    float m_player_velocity[3] = {0.0f, 0.0f, 0.0f};

    void update_input(float dt);
    void update_campaign(float dt);
    void render_frame() const;
    void draw_hud() const;
    void apply_loadout();
    void cycle_weapon(int direction);
    void spawn_initial_entities();
    void apply_weapon_damage(float x, float y, float z, float radius, float damage);
    bool start_host(uint16_t port = 7777);
    bool start_join(const std::string& host, uint16_t port = 7777);
};

} // namespace tehi
