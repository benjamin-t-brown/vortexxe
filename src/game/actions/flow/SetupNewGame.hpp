#pragma once

#include "LibHTML.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/flow/SetupNewLevel.hpp"
#include "game/actions/spawn/SpawnNewShip.hpp"
#include "game/actions/ui/FadeOut.hpp"
#include "game/actions/ui/PlaySound.hpp"
#include "game/state.h"

namespace program {
namespace actions {

class SetupNewGame : public AbstractAction {
  void act() override {
    State& state = *statePtr;

    state.score = 0;
    state.lives = 2;
    state.shouldPlayMusic = false;
    state.isStarting = true;

    libhtml::notifyGameStarted();
    enqueueAction(
        state, std::make_unique<actions::PlaySound>("vortex_new_game"), 0);
    actions::FadeOut().executeImmediate(statePtr);
    enqueueAction(state, std::make_unique<actions::SpawnNewShip>(), 0);
    enqueueAction(state, std::make_unique<actions::SetupNewLevel>(1), 0);
  }
};
}; // namespace actions
} // namespace program