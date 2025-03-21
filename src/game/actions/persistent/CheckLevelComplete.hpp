#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/flow/SetInputMode.hpp"
#include "game/actions/flow/SetupNewLevel.hpp"
#include "game/actions/ship/ModifyScoreAndBonus.hpp"
#include "game/actions/ship/SetBonusAfterMultiplier.hpp"
#include "game/actions/spawn/SpawnTextParticle.hpp"
#include "game/state.h"
#include "lib/sdl2wrapper/Localization.h"
#include <memory>

namespace program {
namespace actions {

class CheckLevelComplete : public AbstractAction {
protected:
  void act() override {
    State& state = *statePtr;

    if (state.inputState == InputState::LEVEL) {
      int numDestructibleAsteroids = 0;

      for (auto& asteroid : state.asteroids) {
        if (asteroid->type != METAL) {
          numDestructibleAsteroids++;
        }
      }

      if (numDestructibleAsteroids == 0) {
        state.parallelActions.clear();
        state.powerups.clear();
        state.ship.wantToShield = false;
        state.ship.isAccelerate = false;
        enqueueAction(
            state,
            std::make_unique<actions::PlaySound>("vortex_wave_completed"),
            0);
        enqueueAction(state,
                      std::make_unique<actions::SetInputMode>(
                          InputState::POST_LEVEL_WAITING_FOR_END_TRANSITION),
                      2000);
        enqueueAction(state,
                      std::make_unique<actions::SetInputMode>(
                          InputState::POST_LEVEL_WAITING_FOR_SCORE_TO_SHOW),
                      2000);
        if (state.bonus > 0) {
          int nextBonus = state.bonus * state.bonusMult;
          if (state.bonusMult > 1) {
            enqueueAction(
                state, std::make_unique<actions::PlaySound>("score_add"), 0);
            enqueueAction(
                state,
                std::make_unique<actions::SetBonusAfterMultiplier>(nextBonus),
                1000);
          } else {
            enqueueAction(
                state,
                std::make_unique<actions::SetBonusAfterMultiplier>(nextBonus),
                100);
          }
          int intermediateBonus = nextBonus;
          while (intermediateBonus > 0) {
            int targetMod = std::min(1000, std::max(100, intermediateBonus));
            int mod = std::min(targetMod, intermediateBonus);
            intermediateBonus -= mod;
            enqueueAction(
                state, std::make_unique<actions::PlaySound>("score_add"), 0);
            enqueueAction(state,
                          std::make_unique<actions::ModifyScoreAndBonus>(mod),
                          100);
          }
          enqueueAction(
              state, std::make_unique<actions::ModifyScoreAndBonus>(0), 500);
          int scoreAddedThisLevel = state.score - state.scoreLevelStart;
          Logger().get() << "Score this level " << scoreAddedThisLevel
                         << Logger::endl;
          if (scoreAddedThisLevel > 3000) {
            enqueueAction(
                state, std::make_unique<actions::ModifyScoreAndBonus>(0), 1000);
            enqueueAction(
                state,
                std::make_unique<actions::SpawnTextParticle>(
                    "+1000 " + sdl2w::L10n::trans(
                                   LOCSTR("Efficiency Bonus")),
                    SDL_Color{100, 100, 255, 255},
                    std::make_pair(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 100)),
                0);
            enqueueAction(
                state,
                std::make_unique<actions::PlaySound>("excellent_job_tim"),
                2000);
          } else {
            enqueueAction(state, nullptr, 2000);
          }
        }
        enqueueAction(state,
                      std::make_unique<actions::SetupNewLevel>(state.level + 1),
                      0);
      }
    }
  }
};

} // namespace actions
} // namespace program
