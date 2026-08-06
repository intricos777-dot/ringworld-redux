#include "map.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace tehi {

bool MapData::load(const std::string& path) {
    FILE* f = fopen(path.c_str(), "r");
    if (!f) {
        std::printf("[Map] Failed to open: %s\n", path.c_str());
        return false;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buf = (char*)malloc(size + 1);
    if (!buf) { fclose(f); return false; }
    fread(buf, 1, size, f);
    buf[size] = '\0';
    fclose(f);

    std::printf("[Map] Loaded %s (%ld bytes)\n", path.c_str(), size);

    // Extract spawn coordinates from JSON
    const char* p = strstr(buf, "\"x\" : ");
    if (p) {
        sscanf(p, "\"x\" : %f", &m_player_spawn.position[0]);
    }
    p = strstr(buf, "\"y\" : ");
    if (p) {
        sscanf(p, "\"y\" : %f", &m_player_spawn.position[1]);
    }
    p = strstr(buf, "\"z\" : ");
    if (p) {
        sscanf(p, "\"z\" : %f", &m_player_spawn.position[2]);
    }

    // Count waypoints
    const char* wp = buf;
    while ((wp = strstr(wp, "\"id\" : ")) != nullptr) {
        wp += 7;
        uint32_t id = 0;
        sscanf(wp, "%u", &id);
        if (id > 0 && id <= 4) {
            Waypoint w;
            w.id = id;
            const char* xp = strstr(wp, "\"x\" : ");
            if (xp) sscanf(xp, "\"x\" : %f", &w.position[0]);
            const char* yp = strstr(wp, "\"y\" : ");
            if (yp) sscanf(yp, "\"y\" : %f", &w.position[1]);
            const char* zp = strstr(wp, "\"z\" : ");
            if (zp) sscanf(zp, "\"z\" : %f", &w.position[2]);
            m_waypoints.push_back(w);
        }
    }

    // Count objectives
    const char* obj = buf;
    while ((obj = strstr(obj, "\"description\"")) != nullptr) {
        Objective o;
        o.id = (uint32_t)(m_objectives.size() + 1);
        const char* desc_start = strstr(obj, "\" : \"");
        if (desc_start) {
            desc_start += 4;
            const char* desc_end = strchr(desc_start, '\"');
            if (desc_end) {
                std::string desc(desc_start, desc_end - desc_start);
                o.description = desc;
                m_objectives.push_back(o);
            }
        }
        obj += 1;
    }

    // Count dialogue lines
    const char* dial = buf;
    while ((dial = strstr(dial, "\"trigger\"")) != nullptr) {
        DialogueLine d;
        const char* trig = strstr(dial, "\" : \"");
        if (trig) {
            trig += 5;
            const char* tend = strchr(trig, '\"');
            if (tend) d.trigger = std::string(trig, tend - trig);
        }
        const char* spk = strstr(dial, "\"speaker\"");
        if (spk) {
            const char* s = strstr(spk, "\" : \"");
            if (s) {
                s += 5;
                const char* send = strchr(s, '\"');
                if (send) d.speaker = std::string(s, send - s);
            }
        }
        const char* line = strstr(dial, "\"line\"");
        if (line) {
            const char* l = strstr(line, "\" : \"");
            if (l) {
                l += 5;
                const char* lend = strchr(l, '\"');
                if (lend) d.line = std::string(l, lend - l);
            }
        }
        if (!d.trigger.empty() && !d.speaker.empty() && !d.line.empty()) {
            m_dialogue.push_back(d);
        }
        dial += 1;
    }

    // Parse hidden legend spawns
    const char* hs = buf;
    while ((hs = strstr(hs, "\"hidden_spawns\"")) != nullptr) {
        const char* arr = strstr(hs, "[");
        if (!arr) break;
        const char* end = strchr(arr, ']');
        if (!end) break;
        std::string block(arr, end - arr);
        unsigned int hid = 0, htype = 0, hcount = 0;
        float hx = 0.0f, hy = 0.0f, hz = 0.0f;
        if (sscanf(block.c_str(), "{\"id\" : %u", &hid) == 1 &&
            sscanf(block.c_str(), "\"x\" : %f", &hx) == 1 &&
            sscanf(block.c_str(), "\"y\" : %f", &hy) == 1 &&
            sscanf(block.c_str(), "\"z\" : %f", &hz) == 1 &&
            sscanf(block.c_str(), "\"enemy_type\" : %u", &htype) == 1 &&
            sscanf(block.c_str(), "\"count\" : %u", &hcount) == 1) {
            HiddenSpawn hsentry;
            hsentry.id = hid;
            hsentry.position[0] = hx;
            hsentry.position[1] = hy;
            hsentry.position[2] = hz;
            hsentry.enemy_type = htype;
            hsentry.count = hcount;
            m_hidden_spawns.push_back(hsentry);
        }
        hs = end + 1;
    }

    free(buf);
    std::printf("[Map] Parsed %zu waypoints, %zu objectives, %zu dialogue lines\n",
        m_waypoints.size(), m_objectives.size(), m_dialogue.size());
    return true;
}

uint32_t Spawner::spawn_player(float x, float y, float z) {
    std::printf("[World] Spawn player at (%f,%f,%f)\n", x, y, z);
    return 1;
}

uint32_t Spawner::spawn_weapon(uint32_t weapon_type, float x, float y, float z) {
    std::printf("[World] Spawn weapon %u at (%f,%f,%f)\n", weapon_type, x, y, z);
    return 1;
}

uint32_t Spawner::spawn_vehicle(uint32_t vehicle_type, float x, float y, float z) {
    std::printf("[World] Spawn vehicle %u at (%f,%f,%f)\n", vehicle_type, x, y, z);
    return 1;
}

uint32_t Spawner::spawn_enemy(float x, float y, float z, uint32_t enemy_type) {
    std::printf("[World] Spawn enemy type %u at (%f,%f,%f)\n", enemy_type, x, y, z);
    return 1;
}

} // namespace tehi
