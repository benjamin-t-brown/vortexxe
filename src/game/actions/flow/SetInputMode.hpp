#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/flow/NotifyGameComplete.hpp"
#include "game/actions/spawn/SpawnAsteroidsForMenu.hpp"
#include "game/actions/spawn/SpawnTextParticle.hpp"
#include "game/actions/ui/CheckUpdateHighScore.hpp"
#include "game/actions/ui/FadeIn.hpp"
#include "game/actions/ui/PlaySound.hpp"
#include "game/state.h"
#include "game/utils/StructTimer.hpp"
#include "lib/sdl2wrapper/Localization.h"

namespace program {
namespace actions {

class SetInputMode : public AbstractAction {
  void act() override {
    State& state = *statePtr;

    state.inputState = inputState;
    if (inputState == InputState::PRE_LEVEL) {
      state.ship.isDestroyed = false;
    }
    if (inputState == InputState::MENU) {
      structTimerStart(state.menuTimer);
      state.menuInd = 0;
      state.asteroids.clear();
      state.bullets.clear();
      state.homingMines.clear();
      state.particles.clear();
      state.powerups.clear();
      state.saucers.clear();
      state.shouldPlayMusic = true;
      enqueueAction(state, std::make_unique<actions::SpawnAsteroidsForMenu>(), 0);
    }
    if (inputState == InputState::GAME_OVER) {
      state.asteroids.clear();
      state.bullets.clear();
      state.homingMines.clear();
      state.particles.clear();
      state.powerups.clear();
      state.saucers.clear();

      actions::FadeIn().executeImmediate(&state);
      enqueueAction(
          state, std::make_unique<actions::PlaySound>("vortex_level_lose"), 2000);

      state.lastScore = state.score;
      enqueueAction(
          state, std::make_unique<actions::CheckUpdateHighScore>(state.score), 0);
      if (state.score > state.highScore) {
        enqueueAction(state, std::make_unique<actions::PlaySound>("ilikeurbeard"), 1000);
        enqueueAction(state,
                      std::make_unique<actions::SpawnTextParticle>(
                          sdl2w::L10n::trans(LOCSTR("New High Score!")),
                          SDL_Color{255, 255, 255, 255},
                          std::make_pair(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 100)),
                      0);
      }
      enqueueAction(state, std::make_unique<actions::NotifyGameComplete>(), 0);
      enqueueAction(state, std::make_unique<actions::SetInputMode>(InputState::MENU), 0);
    }
  }

public:
  InputState inputState = InputState::WAITING;
  SetInputMode(InputState inputState) : inputState(inputState) {}
};

}; // namespace actions
} // namespace program