#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/flow/SetInputMode.hpp"
#include "game/actions/flow/SetupLevelAfterDestroyed.hpp"
#include "game/actions/ui/FadeOut.hpp"
#include "game/state.h"

namespace program {
namespace actions {

class DoPlayerDestroyed : public AbstractAction {
  void act() override {
    State& state = *statePtr;

    if (state.ship.isDestroyed) {
      return;
    }

    state.parallelActions.clear();
    state.ship.isDestroyed = true;
    state.ship.isAccelerate = false;
    state.ship.wantToShield = false;

    enqueueAction(state,
                  std::make_unique<actions::SetInputMode>(InputState::DEFEATED),
                  2000);
    actions::FadeOut().executeImmediate(&state);
    if (state.lives == 0) {
      enqueueAction(
          state,
          std::make_unique<actions::SetInputMode>(InputState::GAME_OVER),
          0);
    } else {
      enqueueAction(
          state, std::make_unique<actions::SetupLevelAfterDestroyed>(), 500);
    }
  }
};
}; // namespace actions
} // namespace program