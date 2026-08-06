#include "sdl_gl_backend.h"
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <cstdio>

namespace tehi {

SDLGLBackend::SDLGLBackend() = default;
SDLGLBackend::~SDLGLBackend() { shutdown(); }

bool SDLGLBackend::initialize(int width, int height, const char* title) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::fprintf(stderr, "[Renderer] SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    m_window = SDL_CreateWindow(title,
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               width, height,
                               SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!m_window) {
        std::fprintf(stderr, "[Renderer] SDL_CreateWindow failed: %s\n", SDL_GetError());
        return false;
    }
    m_ctx = SDL_GL_CreateContext(m_window);
    if (!m_ctx) {
        std::fprintf(stderr, "[Renderer] SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        return false;
    }
    SDL_GL_SetSwapInterval(1);
    glViewport(0, 0, width, height);
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    m_initialized = true;
    std::printf("[Renderer] SDL+GL backend initialized: %dx%d\n", width, height);
    return true;
}

void SDLGLBackend::shutdown() {
    if (!m_initialized) return;
    if (m_ctx) {
        SDL_GL_DeleteContext(m_ctx);
        m_ctx = nullptr;
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    SDL_Quit();
    m_initialized = false;
    std::printf("[Renderer] SDL+GL backend shutdown\n");
}

void SDLGLBackend::begin_frame() {
    if (!m_initialized) return;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SDLGLBackend::end_frame() {
    if (!m_initialized) return;
}

void SDLGLBackend::present() {
    if (m_window) {
        SDL_GL_SwapWindow(m_window);
    }
}

bool SDLGLBackend::should_close() {
    SDL_Event ev;
    bool close = false;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            close = true;
        }
    }
    return close;
}

void* SDLGLBackend::native_window() {
    return m_window;
}

} // namespace tehi
