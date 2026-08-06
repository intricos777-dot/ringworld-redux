#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace tehi {

struct MissionObjective {
    uint32_t id = 0;
    std::string description;
    bool completed = false;
};

struct Checkpoint {
    float position[3];
    float rotation[3];
};

struct DialogueLine {
    std::string trigger;
    std::string speaker;
    std::string line;
};

class Mission {
public:
    Mission() = default;
    ~Mission() = default;
    bool initialize(const std::string& path);
    bool update(float dt);
    void save_checkpoint();
    bool load_checkpoint();
    const std::vector<MissionObjective>& get_objectives() const { return m_objectives; }
    const std::vector<DialogueLine>& get_dialogue() const { return m_dialogue; }
private:
    std::vector<MissionObjective> m_objectives;
    std::vector<Checkpoint> m_checkpoints;
    std::vector<DialogueLine> m_dialogue;
    uint32_t m_current_checkpoint = 0;
};

} // namespace tehi
