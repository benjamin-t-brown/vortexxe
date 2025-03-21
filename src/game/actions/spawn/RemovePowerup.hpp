
#pragma once

#include "game/actions/AbstractAction.h"
#include "game/state.h"

namespace program {
namespace actions {

class RemovePowerup : public AbstractAction {
  void act() override {
    State& state = *statePtr;

    for (auto it = state.powerups.begin(); it != state.powerups.end(); ++it) {
      if (it->get() == &powerup) {
        state.powerups.erase(it);
        break;
      }
    }
  }

public:
  Powerup& powerup;

  RemovePowerup(Powerup& powerup) : powerup(powerup) {}
};
}; // namespace actions
} // namespace program