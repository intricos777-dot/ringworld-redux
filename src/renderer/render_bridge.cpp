#include "render_bridge.h"
#include <renderer/renderer.h>
#include <cstdio>

namespace tehi {

class ConcreteRenderer final : public te::Renderer {
public:
    bool initialize(void*) override {
        std::printf("[Renderer] Dummy backend initialized\n");
        m_initialized = true;
        return true;
    }
    void shutdown() override {
        if (m_initialized) {
            std::printf("[Renderer] Dummy backend shutdown\n");
            m_initialized = false;
        }
    }
    void begin_frame() override {}
    void end_frame() override {}
    te::Texture* create_texture(const te::TextureDesc&, const void*) override {
        if (!m_initialized) return nullptr;
        std::printf("[Renderer] Dummy texture created\n");
        return nullptr;
    }
    te::Shader* create_shader(const te::ShaderDesc&) override {
        if (!m_initialized) return nullptr;
        std::printf("[Renderer] Dummy shader created\n");
        return nullptr;
    }
    te::RenderPass* create_render_pass(const te::RenderPassDesc&) override {
        if (!m_initialized) return nullptr;
        std::printf("[Renderer] Dummy render pass created\n");
        return nullptr;
    }
    void set_vertex_buffer(te::Texture*, uint32_t) override {}
    void draw(uint32_t v, uint32_t i) override {
        if (!m_initialized) return;
        std::printf("[Renderer] Dummy draw vertices=%u instances=%u\n", v, i);
    }
private:
    bool m_initialized = false;
};

RenderBridge::RenderBridge() = default;
RenderBridge::~RenderBridge() { shutdown(); }

bool RenderBridge::initialize(void* window_handle) {
    m_backend = std::make_unique<ConcreteRenderer>();
    if (!m_backend->initialize(window_handle)) {
        std::fprintf(stderr, "[Renderer] Failed to initialize backend\n");
        return false;
    }
    m_initialized = true;
    return true;
}

void RenderBridge::shutdown() {
    if (!m_initialized) return;
    if (m_backend) m_backend->shutdown();
    m_backend.reset();
    m_initialized = false;
    std::printf("[Renderer] Backend shutdown\n");
}

void RenderBridge::begin_frame() {
    if (!m_initialized) return;
    if (m_backend) m_backend->begin_frame();
}

void RenderBridge::end_frame() {
    if (!m_initialized) return;
    if (m_backend) m_backend->end_frame();
}

void RenderBridge::draw_hud_quad(float x, float y, float w, float h, uint32_t color) {
    if (!m_initialized) return;
    (void)x; (void)y; (void)w; (void)h; (void)color;
    // TODO: textured quad pipeline for HUD bars, reticles, and icons
}

void RenderBridge::present() {
    if (!m_initialized) return;
    // TODO: swap-chain present
}

} // namespace tehi
