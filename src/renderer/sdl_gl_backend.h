#pragma once
#include <cstdint>
#include <SDL2/SDL.h>
#include "player/controller.h"

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
    bool poll_events(PlayerController* controller);
    bool should_close() const { return m_should_close; }
    void* native_window();
private:
    bool m_initialized = false;
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_ctx = nullptr;
    bool m_polled_events = false;
    bool m_should_close = false;
    int m_width = 0;
    int m_height = 0;
};

} // namespace tehi
