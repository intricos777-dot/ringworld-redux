#include "te_renderer_backend.h"
#include "player/controller.h"
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
        cfg.scale_mode = 0;

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
        (void)controller;
        if (!initialized) return;
        // Stub: Twilight Elysium event bridge placeholder.
    }

    void begin_frame() {
        if (!initialized) return;
    }

    void end_frame() {
        if (!initialized) return;
    }

    void present() {
        if (!initialized) return;
    }

    bool should_close() const { return !initialized; }

    te::Engine engine;
    te::ResolutionScaler* scaler = nullptr;
    bool initialized = false;
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
