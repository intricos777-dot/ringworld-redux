#pragma once
#include <cstdint>
#include <SDL2/SDL.h>
#include <GL/gl.h>

namespace tehi {

class SDLGLBackend {
public:
    SDLGLBackend();
    ~SDLGLBackend();
    bool initialize(int width, int height, const char* title);
    void shutdown();
    void begin_frame();
    void end_frame();
    void present();
    bool should_close();
    void* native_window();
private:
    bool m_initialized = false;
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_ctx = nullptr;
};

} // namespace tehi
