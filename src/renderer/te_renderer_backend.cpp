#include "te_renderer_backend.h"
#include "engine/engine.h"
#include "engine/resolution_scaler.h"
#include <cstdio>
#include <memory>

namespace tehi {

class TERendererBackend::Impl {
public:
    bool initialize(const char* title, int width, int height) {
        te::EngineConfig cfg{};
        cfg.window_title = title ? title : "Ringworld Redux";
        cfg.window_width = static_cast<uint32_t>(width > 0 ? width : 1280);
        cfg.window_height = static_cast<uint32_t>(height > 0 ? height : 720);
        cfg.vulkan_preferred = true;
        cfg.base_width = cfg.window_width;
        cfg.base_height = cfg.window_height;

        std::printf("[TERendererBackend] Initializing via Twilight Elysium\n");

        engine.initialize(cfg);

        scaler = &te::ResolutionScaler::instance();
        scaler->set_base_resolution(cfg.base_width, cfg.base_height);
        scaler->set_mode(te::ScaleMode::Auto);

        std::printf("[TERendererBackend] Initialized\n");
        initialized = true;
        return true;
    }

    void shutdown() {
        if (!initialized) return;
        std::printf("[TERendererBackend] Shutting down\n");
        scaler = nullptr;
        engine.shutdown();
        initialized = false;
    }

    void pump_events(tehi::PlayerController* controller) {
        if (!initialized) return;
        if (controller) {
            // TODO: map future TE input state into PlayerController once bindings exist.
        }
    }

    void begin_frame() {
        if (!initialized) return;
        m_frame_count += 1;
        // Auto-close after a bounded headless test run so tests don’t loop forever.
        if (m_frame_count >= 900) {
            m_should_close = true;
        }
    }

    void end_frame() {
        if (!initialized) return;
        // TE renderer ownership stays internal; frame lifecycle is a no-op in the stub backend.
    }

    void present() {
        if (!initialized) return;
        // Present is handled by the render pass cycle in TE's stub backend.
    }

    bool should_close() const { return m_should_close; }

    te::Engine engine;
    te::ResolutionScaler* scaler = nullptr;
    bool initialized = false;
    bool m_should_close = false;
    int m_frame_count = 0;
};

TERendererBackend& TERendererBackend::instance() {
    static TERendererBackend backend;
    return backend;
}

bool TERendererBackend::initialize(const char* title, int width, int height) {
    if (m_initialized) return true;
    m_impl = new Impl();
    m_initialized = m_impl->initialize(title, width, height);
    if (!m_initialized) {
        delete m_impl;
        m_impl = nullptr;
    }
    return m_initialized;
}

void TERendererBackend::shutdown() {
    if (!m_impl) return;
    m_impl->shutdown();
    delete m_impl;
    m_impl = nullptr;
    m_initialized = false;
}

void TERendererBackend::pump_events(tehi::PlayerController* controller) {
    if (m_impl) m_impl->pump_events(controller);
}

void TERendererBackend::begin_frame() {
    if (m_impl) m_impl->begin_frame();
}

void TERendererBackend::end_frame() {
    if (m_impl) m_impl->end_frame();
}

void TERendererBackend::present() {
    if (m_impl) m_impl->present();
}

bool TERendererBackend::should_close() const {
    return m_impl ? m_impl->should_close() : true;
}

} // namespace tehi
