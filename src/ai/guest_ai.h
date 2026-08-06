#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace tehi {

enum class GuestAIType : uint32_t {
    Shield = 0,
    Spear,
    Sword
};

struct GuestAITraits {
    float health = 100.0f;
    float shield = 50.0f;
    float damage = 20.0f;
    float move_speed = 1.0f;
    float support_radius = 5.0f;
    const char* role;
};

struct GuestAIPersonality {
    const char* name;
    const char* accent_region;
    const char* speaking_style;
    float aggression;
    float caution;
    float humor;
};

class GuestAI {
public:
    GuestAI();
    ~GuestAI();
    bool initialize(uint32_t id, GuestAIType type, const char* personality_name);
    void update(float dt);
    void deploy();
    void retreat();
    float get_health() const { return m_health; }
    float get_shield() const { return m_shield; }
    const char* get_name() const { return m_name; }
    GuestAIType get_type() const { return m_type; }
    bool is_deployed() const { return m_deployed; }
    void say(const char* line) const;
private:
    uint32_t m_id = 0;
    GuestAIType m_type = GuestAIType::Shield;
    float m_health = 100.0f;
    float m_shield = 50.0f;
    float m_damage = 20.0f;
    float m_move_speed = 1.0f;
    float m_support_radius = 5.0f;
    char m_name[32];
    char m_accent[32];
    char m_speaking_style[64];
    bool m_deployed = false;
    float m_cooldown = 0.0f;
};

} // namespace tehi
