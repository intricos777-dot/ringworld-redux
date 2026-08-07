#pragma once
#include <cstdint>

namespace te {
struct EngineConfig;
class Engine;
class ResolutionScaler;
}

namespace tehi {
class PlayerController;
}

namespace tehi {

class TERendererBackend {
public:
    TERendererBackend() = default;
    ~TERendererBackend() = default;
    TERendererBackend(const TERendererBackend&) = delete;
    TERendererBackend& operator=(const TERendererBackend&) = delete;

    static TERendererBackend& instance();

    bool initialize(const char* title, int width, int height);
    void shutdown();
    void pump_events(tehi::PlayerController* controller);
    void begin_frame();
    void end_frame();
    void present();
    bool should_close() const;

private:
    class Impl;
    Impl* m_impl = nullptr;
    bool m_initialized = false;
    bool m_should_close = false;
};

} // namespace tehi
