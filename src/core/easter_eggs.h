#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <random>

namespace tehi {

enum class EasterEggType : uint32_t {
    WarthogLaunch,
    BansheeSuicide,
    InfiniteAmmo,
    GhostFlip,
    InvisibleScarab,
    RocketJump,
    SpartanFlip,
    BansheeBoost,
    MeleeLunge,
    GlitchText
};

struct EasterEgg {
    EasterEggType type;
    const char* name;
    const char* description;
    float chance;
};

class EasterEggSystem {
public:
    EasterEggSystem();
    ~EasterEggSystem() = default;

    void update(float dt);
    bool try_trigger();
    void trigger(EasterEggType type);
    void set_enabled(bool enabled) { m_enabled = enabled; }
    bool is_enabled() const { return m_enabled; }

    const std::vector<EasterEgg>& get_eggs() const { return m_eggs; }

private:
    std::vector<EasterEgg> m_eggs;
    std::mt19937 m_rng;
    std::uniform_real_distribution<float> m_dist;
    bool m_enabled = true;
    float m_cooldown = 0.0f;
};

EasterEggSystem& get_easter_egg_system();

} // namespace tehi
