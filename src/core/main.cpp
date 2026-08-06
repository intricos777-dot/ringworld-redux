#include "game.h"
#include "campaign/roster.h"
#include "campaign/companion.h"
#include <cstdio>

int main() {
    tehi::Game& game = tehi::Game::instance();
    if (!game.initialize()) {
        std::fprintf(stderr, "Failed to initialize game\n");
        return 1;
    }

    auto& roster = tehi::Roster::instance();
    roster.load();
    const auto* hero = roster.get_protagonist();
    if (hero) {
        std::printf("[main] Protagonist: %s | Callsign: %s | Branch: %s | Rank: %s\n",
            hero->full_name, hero->callsign, (hero->branch == tehi::Branch::Special ? "Special" : "Standard"), hero->rank);
    }

    tehi::StoryCompanion lia;
    lia.initialize();
    lia.say("I am with you. Let us move forward.");

    std::printf("[main] game initialized\n");
    game.run();
    game.shutdown();
    return 0;
}
