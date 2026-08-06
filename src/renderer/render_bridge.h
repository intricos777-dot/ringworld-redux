#pragma once
#include <cstdint>
#include <memory>

namespace te {
class Renderer;
class Texture;
class Shader;
class RenderPass;
}

namespace tehi {

class RenderBridge {
public:
    RenderBridge();
    ~RenderBridge();
    bool initialize(void* window_handle);
    void shutdown();
    void begin_frame();
    void end_frame();
    void draw_hud_quad(float x, float y, float w, float h, uint32_t color);
    void present();
private:
    std::unique_ptr<te::Renderer> m_backend;
    std::unique_ptr<te::Texture> m_white_tex;
    std::unique_ptr<te::Shader> m_shader;
    std::unique_ptr<te::RenderPass> m_pass;
    bool m_initialized = false;
};

} // namespace tehi
