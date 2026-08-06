#pragma once
#include <cstdint>
#include <string>

namespace tehi {

enum class EnemyType : uint32_t { Grey, Reptilian, CthulhuSquid };

struct EnemyDesc {
    EnemyType type;
    char name[24];
    float health = 100.0f;
    float shield = 0.0f;
    float speed = 1.0f;
    float damage = 10.0f;
    uint32_t score = 100;
};

class Enemy {
public:
    Enemy() = default;
    ~Enemy() = default;
    bool initialize(const EnemyDesc& desc);
    void take_damage(float amount);
    bool is_alive() const { return m_health > 0.0f; }
    void update(float dt);
    const char* get_name() const { return m_desc.name; }
    float get_health() const { return m_health; }
private:
    EnemyDesc m_desc;
    float m_health = 0.0f;
    float m_shield = 0.0f;
};

} // namespace tehi
