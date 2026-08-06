#pragma once
#include <vector>
#include <memory>
#include <cstdint>

namespace tehi {

class Entity;
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
    void update_campaign(float dt);
    void render_frame() const;
};

} // namespace tehi
