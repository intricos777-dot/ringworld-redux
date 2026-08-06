#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace tehi {

struct SaveRecord {
    float position[3];
    float rotation[3];
    uint32_t health = 100;
    uint32_t shield = 0;
    uint32_t mission_index = 0;
    uint32_t flags = 0;
};

class SaveSystem {
public:
    SaveSystem();
    ~SaveSystem();
    bool initialize();
    bool save(const char* path);
    bool load(const char* path);
    const SaveRecord& get_record() const { return m_record; }
    void set_record(const SaveRecord& rec) { m_record = rec; }
    bool self_heal();
private:
    SaveRecord m_record;
    SaveRecord m_shadow;
    bool m_corrupted = false;
    uint32_t compute_checksum(const SaveRecord& rec) const;
};

} // namespace tehi
