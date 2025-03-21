#pragma once

#include "game/actions/AbstractAction.h"
#include "game/actions/spawn/SpawnSaucer.hpp"
#include "game/state.h"

namespace program {
namespace actions {

class SpawnSaucersForLevel : public AbstractAction {
  std::pair<double, double> getRandomPosOnLeftSide() {
    int minY = 100;
    int maxY = WINDOW_HEIGHT - 100;

    return {1, static_cast<double>(minY + (rand() % (maxY - minY)))};
  }

  bool getNumSaucersToSpawn() {
    if (level > 4) {
      return 2;
    }
    if (level >= 2) {
      return 1;
    }
    return 0;
  }

  void act() override {
    State& state = *statePtr;

    const int numSaucers = getNumSaucersToSpawn();

    for (int i = 0; i < numSaucers; i++) {
      int msDelay = 2000 + (rand() % 25000);
      auto pos = getRandomPosOnLeftSide();
      auto speed = std::make_pair<double, double>(0.09, 0.);
      addParallelAction(
          state, std::make_unique<SpawnSaucer>(pos, speed), msDelay);
    }
  }

public:
  const int level;

  SpawnSaucersForLevel(int level) : level(level) {}
};
} // namespace actions
} // namespace program
