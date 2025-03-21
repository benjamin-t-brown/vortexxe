#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/spawn/SpawnHomingMine.hpp"
#include "game/physics.hpp"
#include "game/state.h"

namespace program {
namespace actions {

class SpawnHomingMinesForLevel : public AbstractAction {
  std::pair<double, double> getRandomPosAtEdge() {
    const double heading = rand() % 360;
    // assuming width gte height
    const double distance = static_cast<double>(WINDOW_WIDTH) * 1.2;

    auto [dx, dy] = getVector(heading, distance);
    dx += WINDOW_WIDTH / 2.;
    dy += WINDOW_HEIGHT / 2.;

    return {dx, dy};
  }

  bool getNumMinesForLevel() {
    if (level > 5) {
      return 2;
    }
    if (level >= 3) {
      return 1;
    }
    return 0;
  }

  void act() override {
    State& state = *statePtr;

    const int numHomingMines = getNumMinesForLevel();

    for (int i = 0; i < numHomingMines; i++) {
      int msDelay = 2000 + (rand() % 45000);
      auto pos = getRandomPosAtEdge();
      auto speed = std::make_pair<double, double>(0., 0.);
      addParallelAction(
          state, std::make_unique<SpawnHomingMine>(pos, speed), msDelay);
    }
  }

public:
  const int level;

  SpawnHomingMinesForLevel(int level) : level(level) {}
};
} // namespace actions
} // namespace program
