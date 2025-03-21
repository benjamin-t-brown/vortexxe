
#pragma once

#include "game/actions/AbstractAction.h"
#include "game/state.h"

namespace program {
namespace actions {

class RemoveAsteroid : public AbstractAction {
  void act() override {
    State& state = *statePtr;

    for (auto it = state.asteroids.begin(); it != state.asteroids.end(); ++it) {
      if (it->get() == &asteroid) {
        state.asteroids.erase(it);
        break;
      }
    }
  }

public:
  Asteroid& asteroid;

  RemoveAsteroid(Asteroid& asteroid) : asteroid(asteroid) {}
};
}; // namespace actions
} // namespace program