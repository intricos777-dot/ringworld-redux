#include "game.h"
#include <cstdio>

int main() {
    tehi::Game& game = tehi::Game::instance();
    if (!game.initialize()) {
        std::fprintf(stderr, "Failed to initialize game\n");
        return 1;
    }
    std::printf("[main] game initialized\n");
    game.run();
    game.shutdown();
    return 0;
}
