#include "game.h"
#include "campaign/campaign.h"
#include "campaign/roster.h"
#include "campaign/companion.h"
#include "campaign/mission.h"
#include "audio/audio_system.h"
#include "ui/hud.h"
#include "core/achievements.h"
#include "core/game_mode.h"
#include <cstdio>

int main() {
    const char* test_fire = std::getenv("RR_TEST_FIRE");
    if (test_fire) {
        std::printf("[main] RR_TEST_FIRE=%s\n", test_fire);
    }
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

    tehi::AudioEngine audio;
    audio.initialize();
    audio.set_master_volume(1.0f);
    auto& music = tehi::get_music_manager();
    if (!music.playlist().empty()) {
        music.play_track(music.playlist()[0].id);
    }

    tehi::AchievementSystem& achievements = tehi::AchievementSystem::instance();
    achievements.initialize();

    tehi::get_game_mode_system().set_mode(tehi::GameMode::Casual);
    if (tehi::get_game_mode_system().is_legend()) {
        std::printf("[Mode] Legend mode active\n");
    }

    tehi::Campaign campaign;
    campaign.initialize();
    if (!campaign.missions().empty()) {
        std::printf("[main] Current mission: %s\n", campaign.current()->name.c_str());
    }

    tehi::HUD hud;
    hud.initialize();
    hud.draw_health(85.0f);
    hud.draw_shield(50.0f);
    hud.draw_ammo(24, 96);
    hud.render();

    std::printf("[main] game initialized\n");
    game.run();
    game.shutdown();
    return 0;
}
