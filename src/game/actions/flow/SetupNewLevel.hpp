#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/flow/SetInputMode.hpp"
#include "game/actions/spawn/SpawnAsteroidsForLevel.hpp"
#include "game/actions/spawn/SpawnHomingMinesForLevel.hpp"
#include "game/actions/spawn/SpawnPowerupsForLevel.hpp"
#include "game/actions/spawn/SpawnSaucersForLevel.hpp"
#include "game/actions/ui/FadeIn.hpp"
#include "game/actions/ui/PlaySound.hpp"
#include "game/state.h"

namespace program {
namespace actions {

class SetupNewLevel : public AbstractAction {
  void act() override {
    State& state = *statePtr;
    state.level = nextLevel;

    state.bonus = nextLevel * 100;
    state.bonusMult = 1;
    state.scoreLevelStart = state.score;

    state.ship.heading.angle = 0;
    state.ship.heading.turnDirection = NONE;
    state.ship.physics.vx = 0;
    state.ship.physics.vy = 0;
    state.ship.physics.x = WINDOW_WIDTH / 2.;
    state.ship.physics.y = WINDOW_HEIGHT / 2.;

    state.parallelActions.clear();
    state.powerups.clear();
    state.saucers.clear();
    state.bullets.clear();
    state.homingMines.clear();

    state.isStarting = false;

    enqueueAction(
        state, std::make_unique<SetInputMode>(InputState::PRE_LEVEL), 0);
    enqueueAction(
        state, std::make_unique<SpawnAsteroidsForLevel>(nextLevel), 0);
    enqueueAction(state, std::make_unique<SpawnPowerupsForLevel>(nextLevel), 0);
    enqueueAction(state, std::make_unique<SpawnSaucersForLevel>(nextLevel), 0);
    enqueueAction(
        state, std::make_unique<SpawnHomingMinesForLevel>(nextLevel), 0);
    enqueueAction(
        state, std::make_unique<actions::PlaySound>("vortex_level_ready"), 0);
    enqueueAction(state, std::make_unique<actions::FadeIn>(), 0);
  }

public:
  int nextLevel = 1;
  SetupNewLevel(int nextLevel) : nextLevel(nextLevel) {}
};

}; // namespace actions
} // namespace program