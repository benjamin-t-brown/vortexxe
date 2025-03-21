
#pragma once

#include "game/actions/AbstractAction.h"
#include "game/state.h"

namespace program {
namespace actions {

class RemoveHomingMine : public AbstractAction {
  void act() override {
    State& state = *statePtr;

    for (auto it = state.homingMines.begin(); it != state.homingMines.end();
         ++it) {
      if (it->get() == &homingMine) {
        state.homingMines.erase(it);
        break;
      }
    }
  }

public:
  HomingMine& homingMine;

  RemoveHomingMine(HomingMine& homingMine) : homingMine(homingMine) {}
};
}; // namespace actions
} // namespace program