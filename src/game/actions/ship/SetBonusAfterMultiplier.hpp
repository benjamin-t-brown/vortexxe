#pragma once

#include "game/actions/AbstractAction.h"
#include "game/state.h"

namespace program {
namespace actions {

class SetBonusAfterMultiplier : public AbstractAction {
  void act() override {
    State& state = *statePtr;
    state.bonus = v;
    state.bonusMult = 1;
  }

public:
  int v;
  SetBonusAfterMultiplier(int v) : v(v) {}
};
}; // namespace actions
} // namespace program