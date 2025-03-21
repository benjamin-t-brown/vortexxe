#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/flow/SetupNewLevel.hpp"
#include "game/actions/spawn/SpawnNewShip.hpp"
#include "game/state.h"

namespace program {
namespace actions {

class SetupLevelAfterDestroyed : public AbstractAction {
  void act() override {
    State& state = *statePtr;

    state.lives--;

    enqueueAction(state, std::make_unique<actions::SpawnNewShip>(false), 0);
    enqueueAction(
        state, std::make_unique<actions::SetupNewLevel>(state.level), 0);
  }
};
}; // namespace actions
} // namespace program