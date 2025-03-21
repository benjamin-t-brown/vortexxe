#pragma once

#include "game/state.h"
#include "game/utils/StructTimer.hpp"

namespace program {
namespace actions {

class FlipMenu : public AbstractAction {
protected:
  void act() override {
    State& state = *statePtr;

    if (structTimerIsComplete(state.menuTimer)) {
      state.menuInd = 1 - state.menuInd;
      structTimerStart(state.menuTimer);
    }
  }
};

} // namespace actions
} // namespace program
